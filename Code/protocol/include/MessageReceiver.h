#pragma once

#include "Message.h"
#include "Outcome.h"

class MessageReceiver: AllocatorCompatible
{
public:

    enum Error
    {
        kNoMessage,
        kIncomplete,
        kLengthsMismatch,
        kOld
    };

    MessageReceiver() noexcept;
    ~MessageReceiver() noexcept;

    Outcome<Message, MessageReceiver::Error> ReadMessage(Buffer::Reader & aReader) noexcept;

private:

    static constexpr size_t MessageBufferSize = 256;
    
    std::vector<Message *> m_messageBuffer;
};

