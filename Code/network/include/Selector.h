#pragma once

#include "Socket.h"

class Selector
{
public:
    
    Selector(Socket& aSocket);

    bool IsReady() const;

private:

    Socket_t m_sock;
};