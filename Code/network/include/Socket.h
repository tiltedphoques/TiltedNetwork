#pragma once

#include "Network.h"
#include "Outcome.h"
#include "Buffer.h"
#include "Endpoint.h"

class Socket
{
public:

    enum Error
    {
        kInvalidSocket,
        kDiscardError,
        kCallFailure
    };

    struct Packet
    {
        Endpoint Origin;
        Buffer Data;
    };

    Socket();
    ~Socket();

    Outcome<Packet, Error> Receive();
    bool Send(const Packet& aBuffer);
    bool Bind(uint16_t aPort = 0);

    uint16_t GetPort() const;

private:

    static constexpr size_t MaxPacketSize = 1200;

    Socket_t m_sock;
    uint16_t m_port;
};