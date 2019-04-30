#pragma once

#include "Buffer.h"
#include "Outcome.h"


class Connection
{
public:

    struct Header
    {
        char Signature[2];
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
        kTooLarge,
        kUnknownChannel
    };

    Connection();

    bool ProcessPacket(Buffer* apBuffer);
    bool ProcessNegociation(Buffer* apBuffer);

    bool IsNegotiating() const;
    bool IsConnected() const;

protected:

    Outcome<Header, HeaderErrors> ProcessHeader(Buffer::Reader& aReader);

private:

    State m_state;
};