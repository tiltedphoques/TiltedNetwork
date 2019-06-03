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

bool Server::Start(uint16_t aPort) noexcept
{
    if (m_v4Listener.Bind(aPort) == false)
    {
        return false;
    }
    return m_v6Listener.Bind(m_v4Listener.GetPort());
}

uint32_t Server::Update(uint64_t aElapsedMilliSeconds) noexcept
{
    m_connectionManager.Update(aElapsedMilliSeconds);

    return Work();
}

uint16_t Server::GetPort() const noexcept
{
    return m_v4Listener.GetPort();
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

bool Server::ProcessPacket(Socket::Packet& aPacket) noexcept
{
    auto pConnection = m_connectionManager.Find(aPacket.Remote);
    if (!pConnection)
    {
        if (!m_connectionManager.IsFull())
        {
            Connection connection(*this, aPacket.Remote);
            m_connectionManager.Add(std::move(connection));
            pConnection = m_connectionManager.Find(aPacket.Remote);

            if (pConnection)
            {
                if (pConnection->ProcessNegociation(&aPacket.Payload))
                {
                    // OnClientConnected
                    return true;
                }
            }
        }

        return false;
    }
    else if (pConnection->IsNegotiating())
    {
        if (pConnection->ProcessNegociation(&aPacket.Payload))
        {
            // OnClientConnected
            return true;
        }

        return false;
    }
    else if (pConnection->IsConnected())
    {
        return OnPacketReceived(Buffer::Reader(&aPacket.Payload));
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