#pragma once

#include "Network.h"
#include "Outcome.h"

#include <string>

class Endpoint
{
public:

    enum Type
    {
        kNone,
        kIPv4,
        kIPv6
    };

    Endpoint();
    Endpoint(const std::string& acEndpoint);
    Endpoint(const Endpoint& acRhs);

    bool IsIPv6() const;
    bool IsIPv4() const;
    bool IsValid() const;

    uint16_t GetPort() const;
    const uint8_t* GetIPv4() const;
    uint8_t* GetIPv4();
    const uint16_t* GetIPv6() const;
    uint16_t* GetIPv6();

    Endpoint& operator=(const Endpoint& acRhs);
    bool operator==(const Endpoint& acRhs) const;
    bool operator!=(const Endpoint& acRhs) const;

protected:

    void Parse(const std::string& acEndpoint);

private:

    union
    {
        uint8_t m_ipv4[4];
        uint16_t m_ipv6[8];
    };
    
    Type m_type;
    uint16_t m_port;
};