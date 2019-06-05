#include "Connection.h"
#include "StackAllocator.h"

#include "osrng.h"

struct NullCommunicationInterface : public Connection::ICommunication
{
    bool Send(const Endpoint& acRemote, Buffer aBuffer) override
    {
        (void)acRemote;
        (void)aBuffer;

        return false;
    }
};

static NullCommunicationInterface s_dummyInterface;

static const char* s_headerSignature = "MG";

Connection::Connection(ICommunication& aCommunicationInterface, const Endpoint& acRemoteEndpoint, const bool acIsServer)
    : m_communication{ aCommunicationInterface }
    , m_state{kNegociating}
    , m_timeSinceLastEvent{0}
    , m_remoteEndpoint{acRemoteEndpoint}
    , m_isServer{acIsServer}
    , m_remoteCode{ 0 }
{
    CryptoPP::AutoSeededRandomPool rng;
    m_challengeCode = rng.GenerateWord32();

}

Connection::Connection(Connection&& aRhs) noexcept
    : m_communication{aRhs.m_communication}
    , m_state{std::move(aRhs.m_state)}
    , m_timeSinceLastEvent{std::move(aRhs.m_timeSinceLastEvent)}
    , m_remoteEndpoint{std::move(aRhs.m_remoteEndpoint)}
    , m_isServer{aRhs.m_isServer}
    , m_challengeCode{aRhs.m_challengeCode}
    , m_remoteCode{aRhs.m_remoteCode}
{
    aRhs.m_communication = s_dummyInterface;
    aRhs.m_state = kNone;
    aRhs.m_timeSinceLastEvent = 0;
    aRhs.m_challengeCode = 0;
    aRhs.m_remoteCode = 0;
}

Connection::~Connection()
{
}

Connection& Connection::operator=(Connection&& aRhs) noexcept
{
    m_communication = aRhs.m_communication;
    m_state = aRhs.m_state;
    m_timeSinceLastEvent = aRhs.m_timeSinceLastEvent;
    m_remoteEndpoint = std::move(aRhs.m_remoteEndpoint);
    m_isServer = aRhs.m_isServer;
    m_challengeCode = aRhs.m_challengeCode;
    m_remoteCode = aRhs.m_remoteCode;

    aRhs.m_communication = s_dummyInterface;
    aRhs.m_state = kNone;
    aRhs.m_timeSinceLastEvent = 0;
    aRhs.m_challengeCode = 0;
    aRhs.m_remoteCode = 0;

    return *this;
}

Outcome<uint64_t, Connection::HeaderErrors> Connection::ProcessPacket(Buffer::Reader& aReader)
{   
    auto header = ProcessHeader(aReader);
    if (header.HasError())
    {
        return header.GetError();
    }

    switch (header.GetResult().Type)
    {
    case Header::kNegotiation:
        return ProcessNegociation(aReader);
    case Header::kConnection:
        return ProcessConfirmation(aReader);
    case Header::kPayload:
        m_timeSinceLastEvent = 0;
        break;
    default:
        return kUnknownPacket;
    }

    return header.GetResult().Type;
}

Outcome<uint64_t, Connection::HeaderErrors> Connection::ProcessNegociation(Buffer::Reader& aReader)
{
    if (!m_filter.ReceiveConnect(&aReader))
    {
        // Drop connection if exchange fails
        m_state = Connection::kNone;
        return kBadKey;
    }

    if (m_isServer)
    {
        if (aReader.GetSize() < 1200)
        {
            return kPayloadRequired;
        }

        if (!ReadChallenge(aReader, m_remoteCode))
        {
            return kBadChallenge;
        }
    }
    else if (ReadChallenge(aReader, m_remoteCode))
    {
        // We (client) assume to be connected and send back the challenge code
        m_state = kConnected;
        SendConfirmation();
    }

    return Header::kNegotiation;
}

Outcome<uint64_t, Connection::HeaderErrors> Connection::ProcessConfirmation(Buffer::Reader& aReader)
{
    // We are a server that needs to check clients' challenge
    uint32_t confirmationCode = 0;

    if (aReader.GetSize() < 1200)
    {
        return kPayloadRequired;
    }

    if (ReadChallenge(aReader, confirmationCode))
    {
        m_filter.PreReceive((uint8_t *)&confirmationCode, sizeof(confirmationCode), 0);

        if (confirmationCode == (m_challengeCode ^ m_remoteCode))
        {
            // We got a correct challenge code back
            m_state = kConnected;
            return Header::kConnection;
        }
        else
        {
            // Wrong challenge code, drop connection
            m_state = Connection::kNone;
            return kBadChallenge;
        }
    }

    return false;
}

bool Connection::IsNegotiating() const
{
    return m_state == kNegociating;
}

bool Connection::IsConnected() const
{
    return m_state == kConnected;
}

Connection::State Connection::GetState() const
{
    return m_state;
}

const Endpoint& Connection::GetRemoteEndpoint() const
{
    return m_remoteEndpoint;
}

void Connection::Update(uint64_t aElapsedMilliseconds)
{
    m_timeSinceLastEvent += aElapsedMilliseconds;

    // Connection is considered timed out if no data is received in 15s (TODO: make this configurable)
    if (m_timeSinceLastEvent > 15 * 1000)
    {
        m_state = kNone;
        return;
    }

    switch (m_state)
    {
    case Connection::kNone:
        break;
    case Connection::kNegociating:
        SendNegotiation();
        break;
    case Connection::kConnected:
        break;
    default:
        break;
    }
}

void Connection::WriteHeader(Buffer::Writer& aWriter, const uint64_t acHeaderType)
{
    Header header;
    header.Signature[0] = s_headerSignature[0];
    header.Signature[1] = s_headerSignature[1];
    header.Version = 1;
    header.Type = acHeaderType;
    header.Length = 0;

    aWriter.WriteBytes((const uint8_t*)header.Signature, 2);
    aWriter.WriteBits(header.Version, 6);
    aWriter.WriteBits(header.Type, 3);
    aWriter.WriteBits(header.Length, 11);
}

void Connection::SendNegotiation()
{
    StackAllocator<1 << 13> allocator;
    auto* pBuffer = allocator.New<Buffer>(m_isServer ? 200 : 1200);

    Buffer::Writer writer(pBuffer);
    WriteHeader(writer, Header::kNegotiation);

    m_filter.PreConnect(&writer);

    WriteChallenge(writer, m_challengeCode);

    m_communication.Send(m_remoteEndpoint, *pBuffer);

    allocator.Delete(pBuffer);
}

void Connection::SendConfirmation()
{
    StackAllocator<1 << 13> allocator;
    auto* pBuffer = allocator.New<Buffer>(1200);

    Buffer::Writer writer(pBuffer);
    WriteHeader(writer, Header::kConnection);

    uint32_t codeToSend = m_challengeCode ^ m_remoteCode;
    m_filter.PostSend((uint8_t *)&codeToSend, sizeof(codeToSend), 0);
    WriteChallenge(writer, codeToSend);

    m_communication.Send(m_remoteEndpoint, *pBuffer);

    allocator.Delete(pBuffer);
}

Outcome<Connection::Header, Connection::HeaderErrors> Connection::ProcessHeader(Buffer::Reader& aReader)
{
    Header header;

    aReader.ReadBytes((uint8_t*)&header.Signature, 2);

    if (header.Signature[0] != 'M' || header.Signature[1] != 'G')
        return kBadSignature;

    aReader.ReadBits(header.Version, 6);

    if (header.Version != 1)
        return kBadVersion;

    aReader.ReadBits(header.Type, 3);

    if (header.Type >= Header::kCount)
        return kBadPacketType;

    aReader.ReadBits(header.Length, 11);

    if (header.Length > 1200)
        return kTooLarge;

    return header;
}

bool Connection::WriteChallenge(Buffer::Writer &aWriter, uint32_t aCode)
{
    return aWriter.WriteBytes((uint8_t *)&aCode, sizeof(m_challengeCode));
}

bool Connection::ReadChallenge(Buffer::Reader &aReader, uint32_t &aCode)
{
    return aReader.ReadBytes((uint8_t *)&aCode, sizeof(m_challengeCode));
}
