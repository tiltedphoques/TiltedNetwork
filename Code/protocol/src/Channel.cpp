#include "Channel.hpp"
#include <cassert>

namespace TiltedPhoques
{
    Channel::Channel(Type aType, std::function<void(Buffer*)> aMessageReceivedCallback)
        : m_type(aType)
        , m_messageReceivedCallback(std::move(aMessageReceivedCallback))
    {

    }

    Channel::~Channel()
    {

    }

    void Channel::ProcessPacket(Buffer::Reader* apReader)
    {
        switch (m_type)
        {
        case kUnreliable:
            ProcessUnreliablePacket(apReader);
            break;
        case kReliable:
            ProcessReliablePacket(apReader);
            break;
        case kSequenced:
            ProcessSequencedPacket(apReader);
            break;
        }
    }

    void Channel::ProcessUnreliablePacket(Buffer::Reader* apReader)
    {



    }

    void Channel::ProcessReliablePacket(Buffer::Reader* apReader)
    {
        (void)apReader;
        assert(false);
    }

    void Channel::ProcessSequencedPacket(Buffer::Reader* apReader)
    {
        (void)apReader;
        assert(false);
    }
}
