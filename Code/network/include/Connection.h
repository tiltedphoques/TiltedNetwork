#pragma once

#include "Buffer.h"
#include "Outcome.h"
#include "Endpoint.h"
#include "DHChachaFilter.h"

class Socket;
class Connection
{
public:

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
        uint64_t Type;
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
        kPayloadRequired,
        kBadKey,
        kBadChallenge,
        kDeadConnection
    };

    struct ICommunication
    {
        virtual bool Send(const Endpoint& acRemote, Buffer aBuffer) = 0;
    };

    Connection(ICommunication& aCommunicationInterface, const Endpoint& acRemoteEndpoint, const bool acIsServer=false);
    Connection(const Connection& acRhs) = delete;
    Connection(Connection&& aRhs) noexcept;

    ~Connection();

    Connection& operator=(Connection&& aRhs) noexcept;
    Connection& operator=(const Connection& aRhs) = delete;

    Outcome<uint64_t, Connection::HeaderErrors> ProcessPacket(Buffer::Reader & aReader);
    bool IsNegotiating() const;
    bool IsConnected() const;

    State GetState() const;
    const Endpoint& GetRemoteEndpoint() const;

    uint64_t Update(uint64_t aElapsedMilliseconds);
    void Disconnect();

    void WriteHeader(Buffer::Writer& aWriter, const uint64_t acHeaderType);

protected:

    Outcome<Header, HeaderErrors> ProcessHeader(Buffer::Reader& aReader);
    Outcome<uint64_t, Connection::HeaderErrors> ProcessDisconnection(Buffer::Reader & aReader);
    Outcome<uint64_t, Connection::HeaderErrors> ProcessNegociation(Buffer::Reader & aReader);
    Outcome<uint64_t, Connection::HeaderErrors> ProcessConfirmation(Buffer::Reader & aReader);

    void SendNegotiation();
    void SendConfirmation();

    bool WriteChallenge(Buffer::Writer& aWriter, uint32_t aCode);
    bool ReadChallenge(Buffer::Reader& aReader, uint32_t &aCode);

private:

    ICommunication& m_communication;
    State m_state;
    uint64_t m_timeSinceLastEvent;
    Endpoint m_remoteEndpoint;
    DHChachaFilter m_filter;
    bool m_isServer;
    uint32_t m_challengeCode;
    uint32_t m_remoteCode;
};