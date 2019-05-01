#include "Connection.h"
#include "StackAllocator.h"



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

Connection::Connection(ICommunication& aCommunicationInterface, const Endpoint& acRemoteEndpoint)
    : m_communication{ aCommunicationInterface }
    , m_state{kNegociating}
    , m_timeSinceLastEvent{0}
    , m_remoteEndpoint{acRemoteEndpoint}
{

}

Connection::Connection(Connection&& aRhs) noexcept
    : m_communication{std::move(aRhs.m_communication)}
    , m_state{std::move(aRhs.m_state)}
    , m_timeSinceLastEvent{std::move(aRhs.m_timeSinceLastEvent)}
    , m_remoteEndpoint{std::move(aRhs.m_remoteEndpoint)}
{
    aRhs.m_communication = s_dummyInterface;
    aRhs.m_state = kNone;
    aRhs.m_timeSinceLastEvent = 0;
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

    aRhs.m_communication = s_dummyInterface;
    aRhs.m_state = kNone;
    aRhs.m_timeSinceLastEvent = 0;

    return *this;
}

bool Connection::ProcessPacket(Buffer* apBuffer)
{
    Buffer::Reader reader(apBuffer);
    
    auto header = ProcessHeader(reader);
    if (header.HasError())
        return false;

    m_timeSinceLastEvent = 0;

    return true;
}

bool Connection::ProcessNegociation(Buffer* apBuffer)
{
    Buffer::Reader reader(apBuffer);

    auto header = ProcessHeader(reader);
    if (header.HasError())
        return false;

    return IsNegotiating() || IsConnected();
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

void Connection::SendNegotiation()
{
    Header header;
    header.Signature[0] = s_headerSignature[0];
    header.Signature[1] = s_headerSignature[1];
    header.Version = 1;
    header.Type = Header::kNegotiation;
    header.Length = 0;

    StackAllocator<1 << 13> allocator;
    auto* pBuffer = allocator.New<Buffer>(1200);

    Buffer::Writer writer(pBuffer);
    writer.WriteBytes((const uint8_t*)header.Signature, 2);
    writer.WriteBits(header.Version, 6);
    writer.WriteBits(header.Type, 3);
    writer.WriteBits(header.Length, 11);

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