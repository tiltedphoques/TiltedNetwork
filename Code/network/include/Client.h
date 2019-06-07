#pragma once

#include "Socket.h"
#include "ConnectionManager.h"

class Client : public AllocatorCompatible
    , public Connection::ICommunication
{
public:
    Client(const Endpoint& acRemoteEndpoint);

    void Disconnect() noexcept;
    bool Send(const Endpoint& acRemoteEndpoint, Buffer aBuffer) noexcept override;
    bool SendPayload(uint8_t *apData, size_t aLength) noexcept;

    uint32_t Update(const uint64_t aElapsedMilliSeconds) noexcept;

protected:
    bool ProcessPacket(Socket::Packet& aPacket) noexcept;

    virtual bool OnPacketReceived(const Endpoint& acRemoteEndpoint, Buffer::Reader &acBufferReader) noexcept = 0;
    virtual bool OnConnected(const Endpoint& acRemoteEndpoint) noexcept = 0;
    virtual bool OnDisconnected(const Endpoint& acRemoteEndpoint) noexcept = 0;
    
private:

    Connection m_connection;
    Socket m_socket;
};