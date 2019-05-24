#pragma once

#include "Network.h"
#include "Endpoint.h"
#include "Outcome.h"
#include "Meta.h"

#include <string>
#include <vector>
#include <future>

class Resolver
{
public:
    Resolver(const std::string& acAddress);
    std::vector<Endpoint> GetEndpoints() noexcept;

protected:
    void Parse(const std::string& acAddress) noexcept;
    std::vector<Endpoint> ResolveHostname(const std::string& acHostname) noexcept;

private:
    std::future<std::vector<Endpoint>> m_futureEndpoints;
    std::vector<Endpoint> m_endpoints;
    uint16_t m_port;
};
