#include "Endpoint.h"
#include "Network.h"
#include <cstring>
#include "..\include\Endpoint.h"

Endpoint::Endpoint()
{
    m_port = 0;
    m_type = kNone;
}

Endpoint::Endpoint(const std::string& acIp)
{
    m_port = 0;
    m_type = kNone;

    Parse(acIp);
}

Endpoint::Endpoint(const Endpoint& acRhs)
{
    m_port = acRhs.m_port;
    m_type = acRhs.m_type;
    std::copy(std::begin(acRhs.m_ipv6), std::end(acRhs.m_ipv6), std::begin(m_ipv6));
}

Endpoint::Endpoint(uint32_t aNetIPv4, uint16_t aPort)
    : m_type(kIPv4)
    , m_port(aPort)
{
    m_ipv4[0] = (uint8_t)(aNetIPv4 & 0xFF); aNetIPv4 >>= 8;
    m_ipv4[1] = (uint8_t)(aNetIPv4 & 0xFF); aNetIPv4 >>= 8;
    m_ipv4[2] = (uint8_t)(aNetIPv4 & 0xFF); aNetIPv4 >>= 8;
    m_ipv4[3] = (uint8_t)(aNetIPv4 & 0xFF); aNetIPv4 >>= 8;
}

Endpoint::Endpoint(const uint16_t* acpNetIPv6, uint16_t aPort)
    : m_type(kIPv6)
    , m_port(aPort)
{
    m_ipv6[0] = ntohs(acpNetIPv6[0]);
    m_ipv6[1] = ntohs(acpNetIPv6[1]);
    m_ipv6[2] = ntohs(acpNetIPv6[2]);
    m_ipv6[3] = ntohs(acpNetIPv6[3]);
    m_ipv6[4] = ntohs(acpNetIPv6[4]);
    m_ipv6[5] = ntohs(acpNetIPv6[5]);
    m_ipv6[6] = ntohs(acpNetIPv6[6]);
    m_ipv6[7] = ntohs(acpNetIPv6[7]);
}

bool Endpoint::IsIPv4() const
{
    return m_type == kIPv4;
}

bool Endpoint::IsIPv6() const
{
    return m_type == kIPv6;
}

bool Endpoint::IsValid() const
{
    return m_type != kNone;
}

Endpoint::Type Endpoint::GetType() const
{
    return m_type;
}

void Endpoint::SetPort(uint16_t aPort)
{
    m_port = aPort;
}

uint16_t Endpoint::GetPort() const
{
    return m_port;
}

const uint8_t* Endpoint::GetIPv4() const
{
    return m_ipv4;
}

uint8_t* Endpoint::GetIPv4()
{
    return m_ipv4;
}

const uint16_t* Endpoint::GetIPv6() const
{
    return m_ipv6;
}

uint16_t* Endpoint::GetIPv6()
{
    return m_ipv6;
}

bool Endpoint::ToNetIPv4(uint32_t& aDestination) const
{
    if (IsIPv4() == false) return false;

    aDestination |= uint32_t(m_ipv4[0]);
    aDestination |= uint32_t(m_ipv4[1]) << 8;
    aDestination |= uint32_t(m_ipv4[2]) << 16;
    aDestination |= uint32_t(m_ipv4[3]) << 24;

    return true;
}

bool Endpoint::ToNetIPv6(in6_addr& aDestination) const
{
    if (IsIPv6() == false) return false;

    auto pDest = (uint16_t*)&aDestination;

    pDest[0] = htons(m_ipv6[0]);
    pDest[1] = htons(m_ipv6[1]);
    pDest[2] = htons(m_ipv6[2]);
    pDest[3] = htons(m_ipv6[3]);
    pDest[4] = htons(m_ipv6[4]);
    pDest[5] = htons(m_ipv6[5]);
    pDest[6] = htons(m_ipv6[6]);
    pDest[7] = htons(m_ipv6[7]);

    return true;
}

Endpoint& Endpoint::operator=(const Endpoint& acRhs)
{
    m_type = acRhs.m_type;
    m_port = acRhs.m_port;
    std::copy(std::begin(m_ipv6), std::end(m_ipv6), std::begin(m_ipv6));

    return *this;
}

bool Endpoint::operator==(const Endpoint& acRhs) const
{
    if (m_type != acRhs.m_type || m_port != acRhs.m_port)
        return false;

    return std::memcmp(m_ipv6, acRhs.m_ipv6, IsIPv6() ? 16 : 4) == 0;
}

bool Endpoint::operator!=(const Endpoint& acRhs) const
{
    return !this->operator==(acRhs);
}

void Endpoint::Parse(const std::string& acEndpoint)
{
    std::string endpoint(acEndpoint);
    // If we see an IPv6 start character
    if (endpoint[0] == '[')
    {
        auto endChar = endpoint.rfind(']');
        if (endChar != std::string::npos)
        {
            if(endChar + 3 <= endpoint.size() && endpoint[endChar + 1] == ':')
                m_port = std::atoi(&endpoint[endChar + 2]);

            endpoint[endChar] = '\0';
        }

        in6_addr sockaddr6;
        if (inet_pton(AF_INET6, &endpoint[1], &sockaddr6) == 1)
        {
            new (this) Endpoint((uint16_t*)& sockaddr6, m_port);
        }
    }
    else
    {
        auto endChar = endpoint.rfind(':');
        if (endChar != std::string::npos && endChar + 2 <= endpoint.size())
        {
            m_port = std::atoi(&endpoint[endChar + 1]);
            endpoint[endChar] = '\0';
        }

        sockaddr_in sockaddr;
        if (inet_pton(AF_INET, &endpoint[0], &sockaddr.sin_addr) == 1)
        {
            new (this) Endpoint(sockaddr.sin_addr.s_addr, m_port);
        }
        else
        {
            m_port = 0;
            m_type = kNone;
        }
    }

    
}