#include "catch.hpp"

#include "Socket.h"

TEST_CASE("Networking", "[network]")
{
    InitializeNetwork();

    Socket sock;

    ShutdownNetwork();
}

TEST_CASE("Endpoint", "[network.endpoint]")
{
    GIVEN("An empty endpoint")
    {
        Endpoint endpoint;
        REQUIRE(endpoint.IsValid() == false);
    }
    GIVEN("An IPv4")
    {
        Endpoint endpoint("127.0.0.1");
        REQUIRE(endpoint.IsIPv4() == true);
        REQUIRE(endpoint.GetPort() == 0);
        REQUIRE(endpoint.GetIPv4()[0] == 127);
        REQUIRE(endpoint.GetIPv4()[1] == 0);
        REQUIRE(endpoint.GetIPv4()[2] == 0);
        REQUIRE(endpoint.GetIPv4()[3] == 1);
    }
    GIVEN("An IPv4 with a port")
    {
        Endpoint endpoint("127.0.0.1:12345");
        REQUIRE(endpoint.IsIPv4() == true);
        REQUIRE(endpoint.GetPort() == 12345);
        REQUIRE(endpoint.GetIPv4()[0] == 127);
        REQUIRE(endpoint.GetIPv4()[1] == 0);
        REQUIRE(endpoint.GetIPv4()[2] == 0);
        REQUIRE(endpoint.GetIPv4()[3] == 1);
    }
    GIVEN("A bad IPv4")
    {
        Endpoint endpoint("127.0.1");
        REQUIRE(endpoint.IsValid() == false);
        REQUIRE(endpoint.GetPort() == 0);
    }
    GIVEN("A bad IP")
    {
        Endpoint endpoint("lol");
        REQUIRE(endpoint.IsValid() == false);
        REQUIRE(endpoint.GetPort() == 0);
    }
    GIVEN("An IPv6")
    {
        Endpoint endpoint("[2001:0db8:85a3:0000:0000:8a2e:0370:7334]");
        REQUIRE(endpoint.IsIPv6() == true);
        REQUIRE(endpoint.GetPort() == 0);
        REQUIRE(endpoint.GetIPv6()[0] == 0x2001);
        REQUIRE(endpoint.GetIPv6()[1] == 0x0db8);
        REQUIRE(endpoint.GetIPv6()[2] == 0x85a3);
        REQUIRE(endpoint.GetIPv6()[3] == 0x0000);
        REQUIRE(endpoint.GetIPv6()[4] == 0x0000);
        REQUIRE(endpoint.GetIPv6()[5] == 0x8a2e);
        REQUIRE(endpoint.GetIPv6()[6] == 0x0370);
        REQUIRE(endpoint.GetIPv6()[7] == 0x7334);
    }
    GIVEN("An IPv6 with a port")
    {
        Endpoint endpoint("[2001:0db8:85a3:0000:0000:8a2e:0370:7334]:12345");
        REQUIRE(endpoint.IsIPv6() == true);
        REQUIRE(endpoint.GetPort() == 12345);
        REQUIRE(endpoint.GetIPv6()[0] == 0x2001);
        REQUIRE(endpoint.GetIPv6()[1] == 0x0db8);
        REQUIRE(endpoint.GetIPv6()[2] == 0x85a3);
        REQUIRE(endpoint.GetIPv6()[3] == 0x0000);
        REQUIRE(endpoint.GetIPv6()[4] == 0x0000);
        REQUIRE(endpoint.GetIPv6()[5] == 0x8a2e);
        REQUIRE(endpoint.GetIPv6()[6] == 0x0370);
        REQUIRE(endpoint.GetIPv6()[7] == 0x7334);
    }
}