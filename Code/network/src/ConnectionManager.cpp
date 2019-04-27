#include "ConnectionManager.h"

ConnectionManager::ConnectionManager(size_t aMaxConnections)
    : m_maxConnections(aMaxConnections)
{

}

Connection* ConnectionManager::Find(const Endpoint& acEndpoint)
{
    auto itor = m_connections.find(acEndpoint);
    if (itor != std::end(m_connections))
    {
        return &itor->second;
    }

    return nullptr;
}

const Connection* ConnectionManager::Find(const Endpoint& acEndpoint) const
{
    auto itor = m_connections.find(acEndpoint);
    if (itor != std::end(m_connections))
    {
        return &itor->second;
    }

    return nullptr;
}

bool ConnectionManager::IsFull() const
{
    return m_connections.size() >= m_maxConnections;
}
