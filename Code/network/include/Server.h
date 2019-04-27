#pragma once

#include "Socket.h"
#include "ConnectionManager.h"

class Server : public AllocatorCompatible
{
public:

    Server();
    ~Server();

    bool Start(uint16_t aPort);
    uint32_t Update(uint64_t aElapsedMilliSeconds);
    uint16_t GetPort() const;

protected:

    bool ProcessPacket(Socket::Packet& aPacket);

private:

    uint32_t Work();

    Socket m_listener;
    ConnectionManager m_connectionManager;
};