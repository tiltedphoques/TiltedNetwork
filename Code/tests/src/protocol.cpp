#include "catch.hpp"

#include "DHChachaFilter.h"
#include <cstring>


TEST_CASE("Protocol DHChaCha", "[protocol.dhchacha]")
{
    GIVEN("Two DHChaCha filters")
    {
        DHChachaFilter clientFilter;
        DHChachaFilter serverFilter;
        
        static std::string data{ "abcdefhijklmnopqrstuvwxyz" };

        WHEN("Using key exchange")
        {
            Buffer keyExchangePacketClient(1024);
            Buffer keyExchangePacketServer(1024);

            Buffer::Writer writerClient(&keyExchangePacketClient);
            Buffer::Reader readerClient(&keyExchangePacketClient);

            Buffer::Writer writerServer(&keyExchangePacketServer);
            Buffer::Reader readerServer(&keyExchangePacketServer);

            // Client generates connection packet
            REQUIRE(clientFilter.PreConnect(&writerClient) == true);
            // Server reads generated packet
            REQUIRE(serverFilter.ReceiveConnect(&readerClient) == true);
            // Server generates connection response packet
            REQUIRE(serverFilter.PreConnect(&writerServer) == true);
            // Client reads generated packet
            REQUIRE(clientFilter.ReceiveConnect(&readerServer) == true);
        }

        WHEN("Using symmetric encryption")
        {
            Buffer buffer(100);

            for (uint32_t i = 0; i < 16; ++i)
            {
                Buffer::Writer writer(&buffer);
                REQUIRE(writer.WriteBytes((uint8_t*)data.data(), data.length()) == true);

                REQUIRE(clientFilter.PostSend(buffer.GetWriteData(), buffer.GetSize(), i) == true);
                REQUIRE(std::memcmp(buffer.GetData(), data.data(), data.length()) != 0);

                REQUIRE(serverFilter.PreReceive(buffer.GetWriteData(), buffer.GetSize(), i) == true);
                REQUIRE(std::memcmp(buffer.GetData(), data.data(), data.length()) == 0);
            }
        }
    }
}