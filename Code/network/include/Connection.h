#pragma once

#include "Buffer.h"
#include "Outcome.h"
#include "Endpoint.h"
#include "DHChachaFilter.h"
#include "Socket.h"
#include "MessageReceiver.h"

class Socket;
class Connection: public MessageReceiver
{
public:

    typedef uint64_t HeaderType;

    struct Header
    {
        enum
        {
            kNegotiation,
            kConnection,
            kDisconnect,
            kPayload,
            kCount
        };

        char Signature[2];
        uint64_t Version;
        HeaderType Type;
        uint64_t Length;
    };

    enum State
    {
        kNone,
        kNegociating,
        kConnected
    };

    enum HeaderErrors
    {
        kBadSignature,
        kBadVersion,
        kBadPacketType,
        kTooLarge,
        kUnknownChannel,
        kBadKey,
        kBadChallenge,
        kDeadConnection
    };

    struct ICommunication
    {
        virtual bool Send(const Endpoint& acRemote, Buffer aBuffer) = 0;
    };

    Connection(ICommunication& aCommunicationInterface, const Endpoint& acRemoteEndpoint, bool aIsServer=false);
    Connection(const Connection& acRhs) = delete;
    Connection(Connection&& aRhs) noexcept;

    ~Connection();

    Connection& operator=(Connection&& aRhs) noexcept;
    Connection& operator=(const Connection& aRhs) = delete;

    Outcome<HeaderType, Connection::HeaderErrors> ProcessPacket(Buffer::Reader & aReader);
    bool IsNegotiating() const;
    bool IsConnected() const;

    State GetState() const;
    const Endpoint& GetRemoteEndpoint() const;

    State Update(uint64_t aElapsedMilliseconds);
    void Disconnect();

    void WriteHeader(Buffer::Writer& aWriter, HeaderType aHeaderType);

    uint32_t GetNextMessageSeq();

protected:

    Outcome<Header, HeaderErrors> ProcessHeader(Buffer::Reader& aReader);
    Outcome<HeaderType, Connection::HeaderErrors> ProcessDisconnection(Buffer::Reader & aReader);
    Outcome<HeaderType, Connection::HeaderErrors> ProcessNegociation(Buffer::Reader & aReader);
    Outcome<HeaderType, Connection::HeaderErrors> ProcessConfirmation(Buffer::Reader & aReader);

    void SendNegotiation();
    void SendConfirmation();

    bool WriteChallenge(Buffer::Writer& aWriter, uint32_t aCode);
    bool ReadChallenge(Buffer::Reader& aReader, uint32_t &aCode);

private:

    static constexpr size_t MaxNegotiationSize = 200;
    static constexpr size_t ClientPadding = Socket::MaxPacketSize - MaxNegotiationSize;

    ICommunication& m_communication;
    State m_state;
    uint64_t m_timeSinceLastEvent;
    Endpoint m_remoteEndpoint;
    DHChachaFilter m_filter;
    uint32_t m_challengeCode;
    uint32_t m_remoteCode;
    uint32_t m_messageSeq;
    bool m_isServer;
};