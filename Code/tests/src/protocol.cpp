#include "catch.hpp"

#include "DHChachaFilter.h"
#include "Message.h"
#include <cstring>
#include <algorithm>
#include <random>


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

TEST_CASE("Message", "[protocol.message]")
{
    static std::string data{ "abcdefhijklmnopqrstuvwxyz" };

    GIVEN("A simple Message")
    {
        Message message(24, (uint8_t *) data.data(), data.length());
        REQUIRE(message.IsComplete());
        REQUIRE(message.GetSeq() == 24);
        REQUIRE(message.GetLen() == data.length());

        Buffer::Reader reader = message.GetData();
        REQUIRE(reader.GetSize() == message.GetLen());

        Buffer buffer(data.length());
        REQUIRE(reader.ReadBytes(buffer.GetWriteData(), data.length()) == true);
        REQUIRE(std::memcmp(buffer.GetData(), data.data(), data.length()) == 0);
    }

    GIVEN("Message passing")
    {
        Message senderMessage(24, (uint8_t *)data.data(), data.length());
        Buffer buffer(data.length() + Message::HeaderBytes);
        Buffer::Writer writer(&buffer);
        Buffer::Reader reader(&buffer);

        REQUIRE(senderMessage.Write(writer) == data.length());
        
        Message receiverMessage(reader);
        REQUIRE(receiverMessage.IsComplete());
        REQUIRE(receiverMessage.GetSeq() == senderMessage.GetSeq());
        
        reader = receiverMessage.GetData();
        REQUIRE(reader.GetSize() == data.length());
        REQUIRE(reader.ReadBytes(buffer.GetWriteData(), data.length()) == true);
        REQUIRE(std::memcmp(buffer.GetData(), data.data(), data.length()) == 0);
    }

    GIVEN("A fragmented message")
    {
        Message senderMessage(24, (uint8_t *)data.data(), data.length());
        Buffer buffer(1 + Message::HeaderBytes);
        Buffer::Writer writer(&buffer);
        Buffer::Reader reader(&buffer);
        
        auto randomOffsets = std::vector<size_t>(data.length(), 0);
        for (size_t i = 0; i < randomOffsets.size(); i++) randomOffsets[i] = i;

        std::random_device rd;
        std::mt19937 g(rd());
        g.seed(24);
        std::shuffle(randomOffsets.begin(), randomOffsets.end(), g);
        // now we have a really mean test case

        Message *pReceiverMessage = nullptr;

        for (size_t offset : randomOffsets)
        {
            REQUIRE(senderMessage.Write(writer, offset) == 1);
            Message m(reader);
            REQUIRE(m.IsValid() == true);
            REQUIRE(m.IsComplete() == false);
            REQUIRE(m.GetSeq() == senderMessage.GetSeq());
            REQUIRE(m.GetLen() == senderMessage.GetLen());

            if (pReceiverMessage == nullptr)
            {
                pReceiverMessage = new Message(std::move(m));
            }
            else
            {
                Message::Merge(*pReceiverMessage, m);
                REQUIRE(pReceiverMessage->IsValid() == true);
            }

            writer.Reset();
            reader.Reset();
        }

        REQUIRE(pReceiverMessage->IsComplete() == true);

        Buffer completeBuffer(data.length());
        reader = pReceiverMessage->GetData();
        REQUIRE(reader.GetSize() == data.length());
        REQUIRE(reader.ReadBytes(completeBuffer.GetWriteData(), data.length()) == true);
        REQUIRE(std::memcmp(completeBuffer.GetData(), data.data(), data.length()) == 0);

        delete pReceiverMessage;
    }
}