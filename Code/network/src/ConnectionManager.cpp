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

void ConnectionManager::Update(uint64_t aElapsedMilliSeconds, std::function<bool(const Endpoint&)> aDisconnectedCallback)
{
    auto it = m_connections.begin();

    while (it != m_connections.end())
    {
        Connection& connection = (*it).second;

        if (connection.Update(aElapsedMilliSeconds) == Connection::kNone)
        {
            if (aDisconnectedCallback)
            {
                aDisconnectedCallback((*it).first);
            }

            it = m_connections.erase(it);
        }
        else
        {
            it++;
        }
    }
}

void ConnectionManager::Add(Connection aConnection)
{
    m_connections.emplace(aConnection.GetRemoteEndpoint(), std::move(aConnection));
}