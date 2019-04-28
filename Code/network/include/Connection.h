#pragma once

#include "Buffer.h"

class Connection
{
public:

    enum State
    {
        kNone,
        kNegociating,
        kConnected
    };

    Connection();

    bool ProcessPacket(Buffer* apBuffer);
    bool ProcessNegociation(Buffer* apBuffer);

    bool IsNegotiating() const;
    bool IsConnected() const;

protected:

    void ProcessHeader(Buffer::Reader& aReader);

private:

    State m_state;
};