#pragma once

#include "Network.h"
#include "Outcome.h"
#include "Meta.h"

#include <string>

class Endpoint
{
public:

    enum Type : uint8_t
    {
        kNone,
        kIPv4,
        kIPv6
    };

    Endpoint() noexcept;
    Endpoint(Endpoint&& aRhs) noexcept;
    Endpoint(const std::string& acEndpoint) noexcept;
    Endpoint(const Endpoint& acRhs) noexcept;
    Endpoint(uint32_t aNetIPv4, uint16_t aPort) noexcept;
    Endpoint(const uint16_t* acpNetIPv6, uint16_t aPort) noexcept;

    bool IsIPv6() const noexcept;
    bool IsIPv4() const noexcept;
    bool IsValid() const noexcept;
    Type GetType() const noexcept;

    void SetPort(uint16_t aPort) noexcept;
    uint16_t GetPort() const noexcept;
    const uint8_t* GetIPv4() const noexcept;
    uint8_t* GetIPv4() noexcept;
    const uint16_t* GetIPv6() const noexcept;
    uint16_t* GetIPv6() noexcept;

    bool ToNetIPv4(uint32_t& aDestination) const noexcept;
    bool ToNetIPv6(in6_addr& aDestination) const noexcept;

    Endpoint& operator=(const Endpoint& acRhs) noexcept;
    Endpoint& operator=(Endpoint&& aRhs) noexcept;
    bool operator==(const Endpoint& acRhs) const noexcept;
    bool operator!=(const Endpoint& acRhs) const noexcept;

protected:

    void Parse(const std::string& acEndpoint) noexcept;

private:

    union
    {
        uint8_t m_ipv4[4];
        uint16_t m_ipv6[8];
    };

    friend struct std::hash<Endpoint>;
    
    Type m_type;
    uint16_t m_port;
};

namespace std
{
    template<>
    struct hash<Endpoint>
    {
        typedef Endpoint argument_type;
        typedef std::size_t result_type;

        result_type operator()(argument_type const& s) const noexcept
        {
            result_type result = 0;
            hash_combine(result, (uint8_t)s.m_type);
            hash_combine(result, s.m_port);

            result_type tmp = 0;
            if (s.IsIPv6())
            {
                for (auto c : s.m_ipv6)
                {
                    tmp = tmp * 31 + std::hash<uint16_t>()(c);
                }
            }
            else if (s.IsIPv4())
            {
                for (auto c : s.m_ipv4)
                {
                    tmp = tmp * 31 + std::hash<uint8_t>()(c);
                }
            }
            hash_combine(result, tmp);

            return result;
        }
    };
}