#include "Server.h"

Server::Server()
{

}

Server::~Server()
{
}

bool Server::Start(uint16_t aPort)
{
    return m_listener.Bind(aPort);
}

void Server::Update(uint64_t aElapsedMilliSeconds)
{
}

void Server::Work()
{
    while (true)
    {
        auto result = m_listener.Receive();
        if (result.HasError())
        {
            // do some error handling
            continue;
        }


    }
}