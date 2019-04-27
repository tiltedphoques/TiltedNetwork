#pragma once

class Connection
{
public:

    enum State
    {
        kNegociating,
        kConnected
    };

    Connection();

private:

    State m_state;
};