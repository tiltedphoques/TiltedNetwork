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

    void Add(Connection aConnection);

    bool IsFull() const;

    void Update(uint64_t aElapsedMilliSeconds, std::function<bool(const Endpoint&)> aDisconnectedCallback = nullptr);

private:

    std::unordered_map<Endpoint, Connection> m_connections;
    size_t m_maxConnections;
};