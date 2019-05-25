#include "Resolver.h"

#include <utility>

Resolver::Resolver(const std::string & acAddress):
    m_futureEndpoints(std::future<std::vector<Endpoint>>()),
    m_endpoints(std::vector<Endpoint>())
{
    Parse(acAddress);
}

Resolver::Resolver(Resolver && aRhs) noexcept:
    m_futureEndpoints{std::move(aRhs.m_futureEndpoints)},
    m_endpoints{std::move(aRhs.m_endpoints)}
{
}

size_t Resolver::GetSize() noexcept
{
    return GetEndpoints().size();
}

bool Resolver::IsEmpty() noexcept
{
    return GetSize() == 0;
}

Endpoint Resolver::GetEndpoint(const size_t & acIndex) noexcept
{
    return GetEndpoints()[acIndex];
}

Endpoint Resolver::operator[](const size_t & acIndex) noexcept
{
    return GetEndpoint(acIndex);
}

Resolver & Resolver::operator=(Resolver && aRhs) noexcept
{
    m_endpoints = std::move(aRhs.m_endpoints);
    m_futureEndpoints = std::move(aRhs.m_futureEndpoints);

    return *this;
}

Resolver::Iterator Resolver::begin() noexcept
{
    return Iterator(GetEndpoints().cbegin());
}

Resolver::Iterator Resolver::end() noexcept
{
    return Iterator(GetEndpoints().cend());
}

std::vector<Endpoint> & Resolver::GetEndpoints() noexcept
{
    if (m_futureEndpoints.valid()) 
    {
        // std::future::get can only be called once in its lifetime, so we have to keep them all in m_endpoints
        const std::vector<Endpoint> & newEndpoints = m_futureEndpoints.get();
        m_endpoints.insert(std::end(m_endpoints), std::begin(newEndpoints), std::end(newEndpoints));
    }

    return m_endpoints;
}

void Resolver::Parse(std::string aAdress) noexcept
{
    uint16_t port = 0;
    
    if (aAdress.empty())
    {
        return;
    }

    // If we see an IPv6 start character
    if (aAdress[0] == '[')
    {
        auto endChar = aAdress.rfind(']');
        if (endChar != std::string::npos)
        {
            if (endChar + 3 <= aAdress.size() && aAdress[endChar + 1] == ':')
                port = std::atoi(&aAdress[endChar + 2]);

            aAdress[endChar] = '\0';
        }

        in6_addr sockaddr6;
        if (inet_pton(AF_INET6, &aAdress[1], &sockaddr6) == 1)
        {
            m_endpoints.push_back(Endpoint((uint16_t*)& sockaddr6, port));
        }
    }
    else
    {
        auto endChar = aAdress.rfind(':');
        if (endChar != std::string::npos && endChar + 2 <= aAdress.size())
        {
            port = std::atoi(&aAdress[endChar + 1]);
            aAdress[endChar] = '\0';
        }

        sockaddr_in sockaddr;
        if (inet_pton(AF_INET, &aAdress[0], &sockaddr.sin_addr) == 1)
        {
            m_endpoints.push_back(Endpoint(sockaddr.sin_addr.s_addr, port));
        }
        else
        {
            // this call is safe as std::async const & args anyway
            m_futureEndpoints = std::async(std::launch::async, &Resolver::ResolveHostname, this, aAdress, port);
        }
    }
}

std::vector<Endpoint> Resolver::ResolveHostname(const std::string & acHostname, const uint16_t & port) noexcept
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
            endpoints.push_back(Endpoint(((struct sockaddr_in *)r->ai_addr)->sin_addr.s_addr, port));
        }
        else if (r->ai_family == AF_INET6)
        {
            endpoints.push_back(Endpoint((uint16_t *) &((struct sockaddr_in6 *)r->ai_addr)->sin6_addr, port));
        }
    }

    freeaddrinfo(pResolvedAddresses);

    return endpoints;
}
