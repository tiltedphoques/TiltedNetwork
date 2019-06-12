#include "MessageReceiver.h"



MessageReceiver::MessageReceiver() noexcept
    : m_messageBuffer(MessageReceiver::MessageBufferSize, nullptr)
{}


MessageReceiver::~MessageReceiver() noexcept
{
    for (Message *pMessage : m_messageBuffer)
    {
        if (pMessage != nullptr)
            GetAllocator()->Delete<Message>(pMessage);
    }
}

Outcome<Message, MessageReceiver::Error> MessageReceiver::ReadMessage(Buffer::Reader &aReader) noexcept
{
    if (aReader.GetSize() - aReader.GetBytePosition() <= Message::HeaderBytes)
    {
        return MessageReceiver::Error::kNoMessage;
    }

    Message message(aReader);

    if (!message.IsValid())
        return MessageReceiver::Error::kNoMessage;

    if (message.IsComplete())
        return message;


    size_t mPos = message.GetSeq() % MessageReceiver::MessageBufferSize;

    if (m_messageBuffer[mPos] == nullptr)
    {
        m_messageBuffer[mPos] = GetAllocator()->New<Message>(std::move(message));
        return *m_messageBuffer[mPos];
    }

    Message& oldMessage = *m_messageBuffer[mPos];

    if (oldMessage.GetSeq() == message.GetSeq())
    {
        if (oldMessage.GetLen() != message.GetLen())
            return MessageReceiver::Error::kLengthsMismatch;

        if (oldMessage.IsComplete())
            return oldMessage;

        Message::Merge(oldMessage, message);
        return oldMessage;
    }
    else
    {
        if (oldMessage.GetSeq() > message.GetSeq())
            return MessageReceiver::Error::kOld;

        // Buffer entry is stale, replace it
        GetAllocator()->Delete<Message>(m_messageBuffer[mPos]);
        m_messageBuffer[mPos] = GetAllocator()->New<Message>(std::move(message));
        return *m_messageBuffer[mPos];
    }

    // return MessageReceiver::Error::kUndeterminedErrorBecauseIveMissedSomeCondition;
}