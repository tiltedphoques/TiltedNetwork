#include "catch.hpp"

#include "Socket.h"
#include "Resolver.h"
#include "Server.h"
#include "Selector.h"

#include <cstring>
#include <thread>


TEST_CASE("Endpoint", "[network.endpoint]")
{
    InitializeNetwork();

    GIVEN("An empty endpoint")
    {
        Endpoint endpoint;
        REQUIRE(endpoint.IsValid() == false);
    }
}

TEST_CASE("Resolver", "[network.resolver]")
{
    GIVEN("An empty address")
    {
        Resolver resolver("");
        REQUIRE(resolver.IsEmpty() == true);
    }

    GIVEN("An IPv4")
    {
        Resolver resolver("127.0.0.1");
        REQUIRE(resolver.GetSize() == 1);
        Endpoint endpoint = resolver[0];
        REQUIRE(endpoint.IsIPv4() == true);
        REQUIRE(endpoint.GetPort() == 0);
        REQUIRE(endpoint.GetIPv4()[0] == 127);
        REQUIRE(endpoint.GetIPv4()[1] == 0);
        REQUIRE(endpoint.GetIPv4()[2] == 0);
        REQUIRE(endpoint.GetIPv4()[3] == 1);
    }
    GIVEN("An IPv4 with a port")
    {
        Resolver resolver("127.0.0.1:12345");
        REQUIRE(resolver.GetSize() == 1);
        Endpoint endpoint = resolver[0];
        REQUIRE(endpoint.IsIPv4() == true);
        REQUIRE(endpoint.GetPort() == 12345);
        REQUIRE(endpoint.GetIPv4()[0] == 127);
        REQUIRE(endpoint.GetIPv4()[1] == 0);
        REQUIRE(endpoint.GetIPv4()[2] == 0);
        REQUIRE(endpoint.GetIPv4()[3] == 1);
    }
    GIVEN("A bad IPv4")
    {
        Resolver resolver("127.0.0.0.1");
        REQUIRE(resolver.IsEmpty() == true);
    }
    GIVEN("A bad hostname")
    {
        Resolver resolver("lolcalhost777");
        REQUIRE(resolver.IsEmpty() == true);
    }
    GIVEN("A hostname")
    {
        Resolver resolver("localhost");
        REQUIRE(resolver.IsEmpty() == false);

        for (const Endpoint& endpoint : resolver) {
            REQUIRE(endpoint.IsValid() == true);
            REQUIRE(endpoint.GetPort() == 0);

            if (endpoint.IsIPv6())
            {
                REQUIRE(endpoint.GetIPv6()[0] == 0);
                REQUIRE(endpoint.GetIPv6()[1] == 0);
                REQUIRE(endpoint.GetIPv6()[2] == 0);
                REQUIRE(endpoint.GetIPv6()[3] == 0);
                REQUIRE(endpoint.GetIPv6()[4] == 0);
                REQUIRE(endpoint.GetIPv6()[5] == 0);
                REQUIRE(endpoint.GetIPv6()[6] == 0);
                REQUIRE(endpoint.GetIPv6()[7] == 1);
            }
            else
            {
                REQUIRE(endpoint.GetIPv4()[0] == 127);
                REQUIRE(endpoint.GetIPv4()[1] == 0);
                REQUIRE(endpoint.GetIPv4()[2] == 0);
                REQUIRE(endpoint.GetIPv4()[3] == 1);
            }
        }
    }
    GIVEN("A hostname with a port")
    {
        Resolver resolver_original("localhost:12345");
        Resolver resolver(std::move(resolver_original));
        REQUIRE(resolver.IsEmpty() == false);

        for (const Endpoint& endpoint : resolver) {
            REQUIRE(endpoint.IsValid() == true);
            REQUIRE(endpoint.GetPort() == 12345);

            if (endpoint.IsIPv6())
            {
                REQUIRE(endpoint.GetIPv6()[0] == 0);
                REQUIRE(endpoint.GetIPv6()[1] == 0);
                REQUIRE(endpoint.GetIPv6()[2] == 0);
                REQUIRE(endpoint.GetIPv6()[3] == 0);
                REQUIRE(endpoint.GetIPv6()[4] == 0);
                REQUIRE(endpoint.GetIPv6()[5] == 0);
                REQUIRE(endpoint.GetIPv6()[6] == 0);
                REQUIRE(endpoint.GetIPv6()[7] == 1);
            }
            else
            {
                REQUIRE(endpoint.GetIPv4()[0] == 127);
                REQUIRE(endpoint.GetIPv4()[1] == 0);
                REQUIRE(endpoint.GetIPv4()[2] == 0);
                REQUIRE(endpoint.GetIPv4()[3] == 1);
            }
        }
    }
    GIVEN("An IPv6")
    {
        Resolver resolver("[2001:0db8:85a3:0000:0000:8a2e:0370:7334]");
        REQUIRE(resolver.GetSize() == 1);
        Endpoint endpoint = resolver[0];
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
        Resolver resolver("[2001:0db8:85a3:0000:0000:8a2e:0370:7334]:12345");
        REQUIRE(resolver.GetSize() == 1);
        Endpoint endpoint = resolver[0];
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

TEST_CASE("Networking", "[network]")
{
    GIVEN("A basic socket")
    {
        Socket sock;
        REQUIRE(sock.GetPort() == 0);
        REQUIRE(sock.Bind() == true);
        REQUIRE(sock.GetPort() != 0);
    }
    GIVEN("Two sockets v4")
    {
        static const std::string testString = "abcdef";

        Buffer buffer(100);

        Socket client(Endpoint::kIPv4), server(Endpoint::kIPv4);
        REQUIRE(client.Bind());
        REQUIRE(server.Bind());

        Selector clientSelector(client);
        Selector serverSelector(server);

        REQUIRE(clientSelector.IsReady() == false);
        REQUIRE(serverSelector.IsReady() == false);

        Resolver localhostResolver("127.0.0.1");
        Endpoint clientEndpoint = localhostResolver[0];
        Endpoint serverEndpoint = localhostResolver[0];
        clientEndpoint.SetPort(client.GetPort());
        serverEndpoint.SetPort(server.GetPort());

        Buffer::Writer writer(&buffer);
        writer.WriteBytes((const uint8_t*)testString.data(), testString.size());

        Socket::Packet packet{ serverEndpoint, buffer };

        REQUIRE(serverSelector.IsReady() == false);

        REQUIRE(client.Send(packet));

        // Now the server should have available data
        REQUIRE(serverSelector.IsReady());

        auto result = server.Receive();
        REQUIRE(result.HasError() == false);
        auto data = result.GetResult();
        REQUIRE(std::memcmp(data.Payload.GetData(), buffer.GetData(), buffer.GetSize()) == 0);
        REQUIRE(data.Remote.IsIPv4());

        // Reply exactly what we got to the client
        REQUIRE(server.Send(data));
        result = client.Receive();
        REQUIRE(result.HasError() == false);
        data = result.GetResult();
        REQUIRE(std::memcmp(data.Payload.GetData(), buffer.GetData(), buffer.GetSize()) == 0);
        REQUIRE(data.Remote.IsIPv4());
    }
    GIVEN("Two sockets v6")
    {
        static const std::string testString = "abcdef";

        Buffer buffer(100);

        Socket client, server;
        REQUIRE(client.Bind());
        REQUIRE(server.Bind());

        Selector clientSelector(client);
        Selector serverSelector(server);

        REQUIRE(clientSelector.IsReady() == false);
        REQUIRE(serverSelector.IsReady() == false);

        Resolver localhostResolver("[::1]");
        Endpoint clientEndpoint = localhostResolver[0];
        Endpoint serverEndpoint = localhostResolver[0];
        clientEndpoint.SetPort(client.GetPort());
        serverEndpoint.SetPort(server.GetPort());

        Buffer::Writer writer(&buffer);
        writer.WriteBytes((const uint8_t*)testString.data(), testString.size());

        Socket::Packet packet{ serverEndpoint, buffer };

        REQUIRE(serverSelector.IsReady() == false);

        REQUIRE(client.Send(packet));

        // Now the server should have available data
        REQUIRE(serverSelector.IsReady());

        auto result = server.Receive();
        REQUIRE(result.HasError() == false);
        auto data = result.GetResult();
        REQUIRE(std::memcmp(data.Payload.GetData(), buffer.GetData(), buffer.GetSize()) == 0);
        REQUIRE(data.Remote.IsIPv6());

        // Reply exactly what we got to the client
        REQUIRE(server.Send(data));
        result = client.Receive();
        REQUIRE(result.HasError() == false);
        data = result.GetResult();
        REQUIRE(std::memcmp(data.Payload.GetData(), buffer.GetData(), buffer.GetSize()) == 0);
        REQUIRE(data.Remote.IsIPv6());
    }

    GIVEN("A client server model")
    {
        Buffer buffer(100);

        Server server;
        REQUIRE(server.Start(0));

        REQUIRE(server.GetPort() != 0);

        Resolver v4Resolver("127.0.0.1");
        Resolver v6Resolver("[::1]");
        Endpoint serverEndpointv4(v4Resolver[0]);
        Endpoint serverEndpointv6(v6Resolver[0]);

        serverEndpointv6.SetPort(server.GetPort());
        serverEndpointv4.SetPort(server.GetPort());

        Socket clientv6(Endpoint::kIPv6);
        Socket clientv4(Endpoint::kIPv4);

        clientv6.Bind();
        clientv4.Bind();

        /*
        Socket::Packet packetv6{ serverEndpointv6, buffer };
        Socket::Packet packetv4{ serverEndpointv4, buffer };

        REQUIRE(clientv6.Send(packetv6));

        REQUIRE(server.Update(1) == 1);

        REQUIRE(clientv4.Send(packetv4));

        REQUIRE(server.Update(1) == 1);

        REQUIRE(clientv6.Send(packetv6));
        REQUIRE(clientv4.Send(packetv4));

        REQUIRE(server.Update(1) == 2);
        */
    }
}

TEST_CASE("Connection", "[network.connection]")
{
    GIVEN("A connection with a dummy interface")
    {
        static uint32_t s_count{0};
        Resolver localhostResolver("127.0.0.1");
        static Endpoint remoteEndpoint = localhostResolver[0];
        static Buffer buffer;

        remoteEndpoint.SetPort(12345);

        struct DummyCommunication : Connection::ICommunication
        {
            bool Send(const Endpoint& acRemote, Buffer aBuffer) override
            {
                REQUIRE(acRemote == remoteEndpoint);
                buffer = aBuffer;
                ++s_count;
                return true;
            }
        };

        DummyCommunication comm;

        Connection connection(comm, remoteEndpoint);
        Connection connection2(comm, remoteEndpoint);
        REQUIRE(connection.IsNegotiating());

        connection.Update(1);

        REQUIRE(s_count == 1);
        REQUIRE(buffer.GetData()[0] == 'M');
        REQUIRE(buffer.GetData()[1] == 'G');

        REQUIRE(connection2.ProcessNegociation(&buffer));
    }
}

TEST_CASE("Server", "[network.server]")
{
    GIVEN("A client server model")
    {
        Buffer buffer(100);

        Server server;
        REQUIRE(server.Start(0));
        REQUIRE(server.GetPort() != 0);

        Resolver localhostResolver("127.0.0.1");
        Endpoint serverEndpoint = localhostResolver[0];

        serverEndpoint.SetPort(server.GetPort());

        Socket client(Endpoint::kIPv4);
        client.Bind();

        Socket::Packet packet{ serverEndpoint, buffer };
    }
}