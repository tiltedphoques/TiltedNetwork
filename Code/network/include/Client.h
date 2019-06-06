#pragma once

#include "Socket.h"
#include "ConnectionManager.h"
#include "StackAllocator.h"

// TODO migrate code to cpp

class Client : public AllocatorCompatible
    , public Connection::ICommunication
{
public:
    Client(const Endpoint& acRemoteEndpoint)
        : m_connection(*this, acRemoteEndpoint)
        , m_socket(acRemoteEndpoint.GetType(), false)
    {
        m_socket.Bind();
    }

    bool Send(const Endpoint& acRemoteEndpoint, Buffer aBuffer) noexcept override
    {
        Socket::Packet packet{ acRemoteEndpoint, std::move(aBuffer) };
        return m_socket.Send(packet);
    }

    bool SendPayload(uint8_t *apData, size_t aLength) noexcept
    {
        if (!m_connection.IsConnected())
        {
            return false;
        }

        // FIXME memory is allocated (and freed) for every packet sent
        Buffer *pBuffer = GetAllocator()->New<Buffer>(aLength + sizeof(Connection::Header));
        Buffer::Writer writer(pBuffer);
        m_connection.WriteHeader(writer, Connection::Header::kPayload);
        writer.WriteBytes(apData, aLength);
        bool sent = Send(m_connection.GetRemoteEndpoint(), *pBuffer);
        GetAllocator()->Delete<Buffer>(pBuffer);
        return sent;
    }

    uint32_t Update(const uint64_t aElapsedMilliSeconds) noexcept
    {
        uint32_t processedPackets = 0;
        Outcome<Socket::Packet, Socket::Error> result = m_socket.Receive();

        while (!result.HasError())
        {
            // Route packet to a connection
            if (ProcessPacket(result.GetResult()))
                ++processedPackets;

            result = m_socket.Receive();
        }

        m_connection.Update(aElapsedMilliSeconds);

        // TODO error handling
        return processedPackets;
    }

protected:
    bool ProcessPacket(Socket::Packet& aPacket) noexcept
    {
        Buffer::Reader reader(&aPacket.Payload);

        if (m_connection.IsNegotiating())
        {
            if (!m_connection.ProcessPacket(reader).HasError())
            {
                if (m_connection.IsConnected())
                {
                    OnConnected(m_connection.GetRemoteEndpoint());
                }

                return true;
            }

            return false;
        }
        else if (m_connection.IsConnected())
        {
            auto headerType = m_connection.ProcessPacket(reader);

            if (headerType.HasError())
            {
                // TODO error handling
                return false;
            }
            else if (headerType.GetResult() == Connection::Header::kPayload)
            {
                return OnPacketReceived(aPacket.Remote, reader);
            }
        }

        return false;
    }

    virtual bool OnPacketReceived(const Endpoint& acRemoteEndpoint, Buffer::Reader &acBufferReader) noexcept = 0;
    virtual bool OnConnected(const Endpoint& acRemoteEndpoint) noexcept = 0;
    
private:

    Connection m_connection;
    Socket m_socket;
};