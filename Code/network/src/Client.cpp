#include "Client.h"

Client::Client(const Endpoint& acRemoteEndpoint)
    : m_connection(*this, acRemoteEndpoint)
    , m_socket(acRemoteEndpoint.GetType(), false)
{
    m_socket.Bind();
}

void Client::Disconnect() noexcept
{
    if (!m_connection.GetState() == Connection::kNone)
    {
        m_connection.Disconnect();
    }
}

bool Client::Send(const Endpoint& acRemoteEndpoint, Buffer aBuffer) noexcept
{
    Socket::Packet packet{ acRemoteEndpoint, std::move(aBuffer) };
    return m_socket.Send(packet);
}

bool Client::SendPayload(uint8_t *apData, size_t aLength) noexcept
{
    if (!m_connection.IsConnected())
    {
        return false;
    }

    // FIXME memory is allocated (and freed) for every packet sent
    Buffer buffer(Socket::MaxPacketSize);
    uint32_t seq = m_connection.GetNextMessageSeq();
    Message message(seq, apData, aLength);
    Buffer::Writer writer(&buffer);
    size_t bytesWritten = 0;

    while (bytesWritten < aLength)
    {
        writer.Reset();
        m_connection.WriteHeader(writer, Connection::Header::kPayload);
        bytesWritten += message.Write(writer, bytesWritten);
        Send(m_connection.GetRemoteEndpoint(), buffer);
    }

    return true;
}

uint32_t Client::Update(uint64_t aElapsedMilliSeconds) noexcept
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

    if (m_connection.Update(aElapsedMilliSeconds) == Connection::kNone)
    {
        OnDisconnected(m_connection.GetRemoteEndpoint());
    }

    // TODO error handling
    return processedPackets;
}

bool Client::ProcessPacket(Socket::Packet& aPacket) noexcept
{
    Buffer::Reader reader(&aPacket.Payload);

    switch (m_connection.GetState())
    {
    case Connection::kNone:
        break;
    case Connection::kNegociating:
    {
        if (!m_connection.ProcessPacket(reader).HasError())
        {
            if (m_connection.IsConnected())
            {
                OnConnected(m_connection.GetRemoteEndpoint());
            }

            return true;
        }
    }
    break;

    case Connection::kConnected:
    {
        auto headerType = m_connection.ProcessPacket(reader);

        // TODO error handling
        if (!headerType.HasError()
            && (headerType.GetResult() == Connection::Header::kPayload || headerType.GetResult() == Connection::Header::kDisconnect))
        {
            auto messageOutcome = m_connection.ReadMessage(reader);

            while (!messageOutcome.HasError())
            {
                const Message &message = messageOutcome.GetResult();

                if (message.IsComplete())
                    OnMessageReceived(aPacket.Remote, message);

                messageOutcome = m_connection.ReadMessage(reader);
            }

            return true;
        }
    }
    break;

    default:
        // TODO log about new unhandled connection state
        return false;
    }

    return false;
}