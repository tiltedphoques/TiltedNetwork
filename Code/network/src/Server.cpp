#include "Server.h"
#include "Selector.h"

Server::Server()
    : m_connectionManager(64)
{

}

Server::~Server()
{
}

bool Server::Start(uint16_t aPort)
{
    return m_listener.Bind(aPort);
}

uint32_t Server::Update(uint64_t aElapsedMilliSeconds)
{
    return Work();
}

uint16_t Server::GetPort() const
{
    return m_listener.GetPort();
}

bool Server::ProcessPacket(Socket::Packet& aPacket)
{
    auto pConnection = m_connectionManager.Find(aPacket.Remote);
    if (pConnection)
    {
        m_connectionManager.Add(aPacket.Remote, &aPacket.Payload);

        return true;
    }
    else if(m_connectionManager.IsFull() == false)
    {
        // New connection


        return true;
    }

    return false;
}

uint32_t Server::Work()
{
    uint32_t processedPackets = 0;

    Selector selector(m_listener);
    while (selector.IsReady())
    {
        auto result = m_listener.Receive();
        if (result.HasError())
        {
            // do some error handling
            continue;
        }
        else
        {
            // Route packet to a connection
            if(ProcessPacket(result.GetResult()))
                ++processedPackets;
        }
    }

    return processedPackets;
}