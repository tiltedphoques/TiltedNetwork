#pragma once

#include "Socket.h"
#include "ConnectionManager.h"

class Server : public AllocatorCompatible
             , public Connection::ICommunication
{
public:

    Server();
    ~Server();

    bool Start(uint16_t aPort);
    uint32_t Update(uint64_t aElapsedMilliSeconds);
    uint16_t GetPort() const;

    bool Send(const Endpoint& acRemoteEndpoint, Buffer aBuffer) override;

protected:

    bool ProcessPacket(Socket::Packet& aPacket);

private:

    uint32_t Work();

    Socket m_v4Listener, m_v6Listener;
    ConnectionManager m_connectionManager;
};