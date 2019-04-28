#pragma once

#include "Socket.h"
#include "Connection.h"
#include <unordered_map>

class ConnectionManager : public AllocatorCompatible
{
public:

    ConnectionManager(size_t aMaxConnections);

    Connection* Find(const Endpoint& acEndpoint);
    const Connection* Find(const Endpoint& acEndpoint) const;

    void Add(const Endpoint& acEndpoint, Buffer* apConnectionPayload);

    bool IsFull() const;

private:

    std::unordered_map<Endpoint, Connection> m_connections;
    size_t m_maxConnections;
};