#pragma once

#include "Network.h"
#include "Outcome.h"
#include "Buffer.h"

class Socket
{
public:

    enum Error
    {
        kInvalidSocket,
        kDiscardError,
        kCallFailure
    };

    Socket();
    ~Socket();

    Outcome<Buffer, Error> Receive();

private:

    static constexpr size_t MaxPacketSize = 1200;

    Socket_t m_sock;
};