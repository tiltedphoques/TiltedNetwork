#include "Endpoint.h"
#include "Network.h"


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
    std::copy(std::begin(m_ipv6), std::end(m_ipv6), std::begin(m_ipv6));
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

    return std::memcmp(m_ipv6, acRhs.m_ipv6, kIPv6 ? 16 : 4) == 0;
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

        in_addr6 sockaddr6;
        if (inet_pton(AF_INET6, &endpoint[1], &sockaddr6) == 1)
        {
            for (auto i = 0; i < 8; ++i)
            {
                m_ipv6[i] = ntohs(((uint16_t*)& sockaddr6)[i]);
            }

            m_type = kIPv6;
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
            auto ip32 = sockaddr.sin_addr.s_addr;
            m_ipv4[0] = (uint8_t)(ip32 & 0xFF); ip32 >>= 8;
            m_ipv4[1] = (uint8_t)(ip32 & 0xFF); ip32 >>= 8;
            m_ipv4[2] = (uint8_t)(ip32 & 0xFF); ip32 >>= 8;
            m_ipv4[3] = (uint8_t)(ip32 & 0xFF); ip32 >>= 8;

            m_type = kIPv4;
        }
        else
        {
            m_port = 0;
            m_type = kNone;
        }
    }

    
}