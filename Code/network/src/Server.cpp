#include "Server.h"
#include "Selector.h"

Server::Server()
    : m_connectionManager(64)
    , m_v4Listener(Endpoint::kIPv4)
    , m_v6Listener(Endpoint::kIPv6)
{

}

Server::~Server()
{
}

bool Server::Start(const uint16_t acPort) noexcept
{
    if (m_v4Listener.Bind(acPort) == false)
    {
        return false;
    }
    return m_v6Listener.Bind(m_v4Listener.GetPort());
}

uint32_t Server::Update(const uint64_t acElapsedMilliSeconds) noexcept
{
    uint32_t nPackets = Work();
    m_connectionManager.Update(acElapsedMilliSeconds, std::bind(&Server::OnClientDisconnected, this, std::placeholders::_1));
    return nPackets;
}

uint16_t Server::GetPort() const noexcept
{
    return m_v4Listener.GetPort();
}

void Server::Disconnect(const Endpoint& acRemoteEndpoint) noexcept
{
    auto pConnection = m_connectionManager.Find(acRemoteEndpoint);
    if (pConnection && !(pConnection->GetState() == Connection::kNone))
    {
        pConnection->Disconnect();
    }
}

bool Server::Send(const Endpoint& acRemoteEndpoint, Buffer aBuffer) noexcept
{
    Socket::Packet packet{ acRemoteEndpoint, std::move(aBuffer) };

    if (acRemoteEndpoint.IsIPv6())
    {
        return m_v6Listener.Send(packet);
    }

    if (acRemoteEndpoint.IsIPv4())
    {
        return m_v4Listener.Send(packet);
    }

    return false;
}

bool Server::SendPayload(const Endpoint& acRemoteEndpoint, uint8_t *apData, size_t aLength) noexcept
{
    auto pConnection = m_connectionManager.Find(acRemoteEndpoint);
    if (!pConnection || !pConnection->IsConnected())
    {
        return false;
    }

    // FIXME memory is allocated (and freed) for every packet sent
    Buffer *pBuffer = GetAllocator()->New<Buffer>(aLength + 8); // Assuming 8 header bytes
    Buffer::Writer writer(pBuffer);
    pConnection->WriteHeader(writer, Connection::Header::kPayload);
    writer.WriteBytes(apData, aLength);
    bool sent = Send(acRemoteEndpoint, *pBuffer);
    GetAllocator()->Delete<Buffer>(pBuffer);
    return sent;
}

bool Server::ProcessPacket(Socket::Packet& aPacket) noexcept
{
    Buffer::Reader reader(&aPacket.Payload);
    auto pConnection = m_connectionManager.Find(aPacket.Remote);
    if (!pConnection)
    {
        if (!m_connectionManager.IsFull())
        {
            Connection connection(*this, aPacket.Remote, true);
            m_connectionManager.Add(std::move(connection));
            pConnection = m_connectionManager.Find(aPacket.Remote);

            if (pConnection)
            {
                return !pConnection->ProcessPacket(reader).HasError();
                // TODO error handling
            }
        }

        return false;
    }
    else switch (pConnection->GetState())
    {
    case Connection::kNone:
        break;
    case Connection::kNegociating:
    {
        if (!pConnection->ProcessPacket(reader).HasError())
        {
            if (pConnection->IsConnected())
            {
                OnClientConnected(pConnection->GetRemoteEndpoint());
            }

            return true;
        }
    }
    break;

    case Connection::kConnected:
    {
        auto headerType = pConnection->ProcessPacket(reader);

        // TODO error handling
        if (!headerType.HasError() 
            && (headerType.GetResult() == Connection::Header::kPayload || headerType.GetResult() == Connection::Header::kDisconnect))
        {
            return OnPacketReceived(aPacket.Remote, reader);
        }
    }
    break;

    default:
        // TODO log about new unhandled connection state
        return false;
    }

    return false;
}

uint32_t Server::Work() noexcept
{
    uint32_t processedPackets = 0;

    Selector selector(m_v4Listener);
    while (selector.IsReady())
    {
        auto result = m_v4Listener.Receive();
        if (result.HasError())
        {
            // do some error handling
            continue;
        }
        else
        {
            // Route packet to a connection
            if (ProcessPacket(result.GetResult()))
                ++processedPackets;
        }
    }

    Selector selectorv6(m_v6Listener);
    while (selectorv6.IsReady())
    {
        auto result = m_v6Listener.Receive();
        if (result.HasError())
        {
            // do some error handling
            continue;
        }
        else
        {
            // Route packet to a connection
            if (ProcessPacket(result.GetResult()))
                ++processedPackets;
        }
    }

    return processedPackets;
}