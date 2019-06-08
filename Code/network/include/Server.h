#pragma once

#include "Socket.h"
#include "ConnectionManager.h"

class Server : public AllocatorCompatible
             , public Connection::ICommunication
{
public:

    Server();
    ~Server();

    bool Start(const uint16_t acPort) noexcept;
    uint32_t Update(const uint64_t acElapsedMilliSeconds) noexcept;
    uint16_t GetPort() const noexcept;

    void Disconnect(const Endpoint& acRemoteEndpoint) noexcept;
    bool Send(const Endpoint& acRemoteEndpoint, Buffer aBuffer) noexcept override;
    bool SendPayload(const Endpoint& acRemoteEndpoint, uint8_t *apData, size_t aLength) noexcept;

protected:
    virtual bool OnPacketReceived(const Endpoint& acRemoteEndpoint, Buffer::Reader &aBufferReader) noexcept = 0;
    virtual bool OnClientConnected(const Endpoint& acRemoteEndpoint) noexcept = 0;
    virtual bool OnClientDisconnected(const Endpoint& acRemoteEndpoint) noexcept = 0;
    bool ProcessPacket(Socket::Packet& aPacket) noexcept;

private:

    uint32_t Work() noexcept;

    Socket m_v4Listener, m_v6Listener;
    ConnectionManager m_connectionManager;
};