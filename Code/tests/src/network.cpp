#include "catch.hpp"

#include "Socket.h"
#include "Resolver.h"
#include "Server.h"
#include "Selector.h"
#include "Client.h"

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

        Connection clientConnection(comm, remoteEndpoint);
        Connection serverConnection(comm, remoteEndpoint, true);
        REQUIRE(clientConnection.IsNegotiating());
        REQUIRE(clientConnection.Update(1) == Connection::kNegociating);

        REQUIRE(s_count == 1);
        REQUIRE(buffer.GetData()[0] == 'M');
        REQUIRE(buffer.GetData()[1] == 'G');

        Buffer::Reader reader(&buffer);
        REQUIRE(serverConnection.ProcessPacket(reader).GetResult() == Connection::Header::kNegotiation);
        REQUIRE(serverConnection.Update(1) == Connection::kNegociating);
        reader.Reset();

        REQUIRE(clientConnection.ProcessPacket(reader).GetResult() == Connection::Header::kNegotiation);
        REQUIRE(clientConnection.Update(1) == Connection::kConnected);
        REQUIRE(clientConnection.IsConnected());
        reader.Reset();

        REQUIRE(serverConnection.ProcessPacket(reader).GetResult() == Connection::Header::kConnection);
        REQUIRE(serverConnection.Update(1) == Connection::kConnected);
        REQUIRE(serverConnection.IsConnected());
        reader.Reset();

        serverConnection.Disconnect();
        REQUIRE(serverConnection.GetState() == Connection::kNone);
        REQUIRE(clientConnection.ProcessPacket(reader).GetResult() == Connection::Header::kDisconnect);
        REQUIRE(clientConnection.GetState() == Connection::kNone);
        reader.Reset();

        REQUIRE(clientConnection.ProcessPacket(reader).GetError() == Connection::HeaderErrors::kDeadConnection);
        REQUIRE(clientConnection.GetState() == Connection::kNone);
    }
}

TEST_CASE("Server", "[network.server]")
{
    class MyServer : public Server
    {
    public:
        MyServer() :
            Server()
            , m_clients()
        {};

        void SendACK()
        {
            for (auto& client : m_clients)
            {
                SendPayload(client.first, (uint8_t *)&client.second, 4);
            }
        }

        size_t NumClients()
        {
            return m_clients.size();
        }

    protected:
        bool OnPacketReceived(const Endpoint& acRemoteEndpoint, Buffer::Reader &aBufferReader) noexcept override
        {
            uint32_t seq;

            if (aBufferReader.ReadBytes((uint8_t *)&seq, 4))
            {
                if (seq > m_clients[acRemoteEndpoint])
                    m_clients[acRemoteEndpoint] = seq;

                return true;
            }

            return false;
        }

        bool OnClientConnected(const Endpoint& acRemoteEndpoint) noexcept override
        {
            m_clients[acRemoteEndpoint] = 0;
            return true;
        }

        bool OnClientDisconnected(const Endpoint &acRemoteEndpoint) noexcept override
        {
            m_clients.erase(acRemoteEndpoint);
            return true;
        }

    private:
        std::unordered_map<Endpoint, uint32_t> m_clients;
    };

    class MyClient : public Client
    {
    public:
        uint32_t m_seq;
        uint32_t m_lastAck;
        bool m_connected;

        MyClient(const Endpoint& acRemoteEndpoint) :
            Client(acRemoteEndpoint)
            , m_seq{ 0 }
            , m_lastAck{ 0 }
            , m_connected { false }
        {}

        void IncrAndSend()
        {
            if (m_lastAck == m_seq)
                m_seq++;

            SendPayload((uint8_t *)&m_seq, 4);
        }

    protected:
        bool OnPacketReceived(const Endpoint& acRemoteEndpoint, Buffer::Reader &aBufferReader) noexcept override
        {
            if (aBufferReader.ReadBytes((uint8_t *)&m_lastAck, 4))
            {
                return true;
            }

            return false;
        }

        bool OnConnected(const Endpoint& acRemoteEndpoint) noexcept override
        {
            m_connected = true;
            return true;
        }

        bool OnDisconnected(const Endpoint &acRemoteEndpoint) noexcept override
        {
            m_seq = 0;
            m_lastAck = 0;
            m_connected = false;
            return true;
        }
    };

    GIVEN("A client server model")
    {
        static Resolver localhostResolver("127.0.0.1");
        static Endpoint serverEndpoint = localhostResolver[0];
        static MyServer server;

        WHEN("Server is started")
        {
            REQUIRE(server.Start(0));
            REQUIRE(server.GetPort() != 0);
            REQUIRE(server.Update(1) == 0);
            REQUIRE(server.NumClients() == 0);
            serverEndpoint.SetPort(server.GetPort());
        }

        static MyClient client1(serverEndpoint), client2(serverEndpoint), client3(serverEndpoint);

        WHEN("Client 1 connects")
        {
            REQUIRE(client1.m_connected == false);

            REQUIRE(client1.Update(1) == 0);
            REQUIRE(client1.m_connected == false);

            REQUIRE(server.Update(1) == 1);
            REQUIRE(server.NumClients() == 0);

            REQUIRE(client1.Update(1) == 1);
            REQUIRE(client1.m_connected == true);

            REQUIRE(server.Update(1) == 1);
            REQUIRE(server.NumClients() == 1);
        }

        WHEN("Client 1 sends some packets")
        {
            client1.IncrAndSend();
            REQUIRE(client1.m_seq == 1);
            REQUIRE(client1.m_lastAck == 0);
            REQUIRE(server.Update(1) == 1);
            server.SendACK();

            REQUIRE(client1.Update(1) == 1);
            REQUIRE(client1.m_seq == 1);
            REQUIRE(client1.m_lastAck == 1);

            client1.IncrAndSend();
            client1.IncrAndSend();
            client1.IncrAndSend();
            REQUIRE(client1.m_seq == 2);
            REQUIRE(client1.m_lastAck == 1);

            REQUIRE(server.Update(1) == 3);
            server.SendACK();

            REQUIRE(client1.Update(1) == 1);
            REQUIRE(client1.m_lastAck == 2);
        }

        WHEN("More clients connect")
        {
            REQUIRE(client1.m_connected == true);
            REQUIRE(client2.m_connected == false);
            REQUIRE(client2.m_connected == false);
            REQUIRE(server.NumClients() == 1);
            REQUIRE(client2.Update(1) == 0);
            REQUIRE(client3.Update(1) == 0);
            REQUIRE(server.Update(1) == 2);
            REQUIRE(server.NumClients() == 1);
            REQUIRE(client2.Update(1) == 1);
            REQUIRE(client3.Update(1) == 1);
            REQUIRE(client2.m_connected == true);
            REQUIRE(client3.m_connected == true);
            REQUIRE(server.Update(1) == 2);
            REQUIRE(server.NumClients() == 3);
        }

        WHEN("Clients exchange packets")
        {
            client1.IncrAndSend();
            client2.IncrAndSend();
            REQUIRE(client1.m_seq == 3);
            REQUIRE(client1.m_lastAck == 2);
            REQUIRE(client2.m_seq == 1);
            REQUIRE(client2.m_lastAck == 0);
            REQUIRE(server.Update(1) == 2);
            server.SendACK();

            REQUIRE(client1.Update(1) == 1);
            REQUIRE(client2.Update(1) == 1);
            REQUIRE(client3.Update(1) == 1);
            REQUIRE(client1.m_lastAck == 3);
            REQUIRE(client2.m_lastAck == 1);
            REQUIRE(client3.m_lastAck == 0);
        }

        WHEN("Clients 1 and 2 disconnect")
        {
            REQUIRE(server.NumClients() == 3);
            REQUIRE(client1.m_connected == true);
            REQUIRE(client2.m_connected == true);
            
            client1.Disconnect();
            client2.Disconnect();

            REQUIRE(client1.Update(1) == 0);
            REQUIRE(client2.Update(1) == 0);

            REQUIRE(client1.m_connected == false);
            REQUIRE(client2.m_connected == false);

            REQUIRE(server.Update(1) == 2);
            REQUIRE(server.NumClients() == 1);
        }

        WHEN("Client 3 times out")
        {
            REQUIRE(server.NumClients() == 1);
            REQUIRE(client3.m_connected == true);
            REQUIRE(server.Update(60 * 1000) == 0);
            REQUIRE(server.NumClients() == 0);
        }
    }
}