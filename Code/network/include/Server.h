#pragma once

#include "Socket.h"

class Server
{
public:

    Server();
    ~Server();

    bool Start(uint16_t aPort);
    void Update(uint64_t aElapsedMilliSeconds);

private:

    void Work();

    Socket m_listener;
};