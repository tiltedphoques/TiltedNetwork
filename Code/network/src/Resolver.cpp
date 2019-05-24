#include "Resolver.h"

Resolver::Resolver(const std::string & acAddress):
    m_futureEndpoints(std::future<std::vector<Endpoint>>()),
    m_endpoints(std::vector<Endpoint>()),
    m_port(0)
{
    Parse(acAddress);
}

std::vector<Endpoint> Resolver::GetEndpoints() noexcept
{
    if (m_futureEndpoints.valid()) {
        // std::future::get can only be called once in its lifetime, so we have to keep them all in m_endpoints
        const std::vector<Endpoint> & newEndpoints = m_futureEndpoints.get();
        m_endpoints.insert(std::end(m_endpoints), std::begin(newEndpoints), std::end(newEndpoints));
    }

    return m_endpoints;
}

void Resolver::Parse(const std::string & acAddress) noexcept
{
    std::string strAddress(acAddress);
    
    if (acAddress.empty())
    {
        return;
    }

    // If we see an IPv6 start character
    if (strAddress[0] == '[')
    {
        auto endChar = strAddress.rfind(']');
        if (endChar != std::string::npos)
        {
            if (endChar + 3 <= strAddress.size() && strAddress[endChar + 1] == ':')
                m_port = std::atoi(&strAddress[endChar + 2]);

            strAddress[endChar] = '\0';
        }

        in6_addr sockaddr6;
        if (inet_pton(AF_INET6, &strAddress[1], &sockaddr6) == 1)
        {
            m_endpoints.push_back(Endpoint((uint16_t*)& sockaddr6, m_port));
        }
    }
    else
    {
        auto endChar = strAddress.rfind(':');
        if (endChar != std::string::npos && endChar + 2 <= strAddress.size())
        {
            m_port = std::atoi(&strAddress[endChar + 1]);
            strAddress[endChar] = '\0';
        }

        sockaddr_in sockaddr;
        if (inet_pton(AF_INET, &strAddress[0], &sockaddr.sin_addr) == 1)
        {
            m_endpoints.push_back(Endpoint(sockaddr.sin_addr.s_addr, m_port));
        }
        else
        {
            m_futureEndpoints = std::async(std::launch::async, &Resolver::ResolveHostname, this, strAddress.substr(0, endChar));
        }
    }
}

std::vector<Endpoint> Resolver::ResolveHostname(const std::string & acHostname) noexcept
{
    std::vector<Endpoint> endpoints = std::vector<Endpoint>();
    struct addrinfo hints = {};
    struct addrinfo *pResolvedAddresses;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    const int err = getaddrinfo(acHostname.c_str(), nullptr, &hints, &pResolvedAddresses);

    if (err != 0)
    {
        // printf("%s\n", gai_strerror(err));
        return endpoints;
    }

    for (struct addrinfo *r = pResolvedAddresses; r != nullptr; r = r->ai_next)
    {
        if (r->ai_family == AF_INET)
        {
            endpoints.push_back(Endpoint(((struct sockaddr_in *)r->ai_addr)->sin_addr.s_addr, m_port));
        }
        else if (r->ai_family == AF_INET6)
        {
            endpoints.push_back(Endpoint((uint16_t *) &((struct sockaddr_in6 *)r->ai_addr)->sin6_addr, m_port));
        }
    }

    freeaddrinfo(pResolvedAddresses);

    return endpoints;
}
