#pragma once

#include <list>

#include "Buffer.h"
#include "Allocator.h"

class Message
{
public:
    static constexpr uint8_t MessageLenBits = 16;
    static constexpr size_t MaxMessageSize = (1 << MessageLenBits) - 1;
    static constexpr size_t HeaderBytes = sizeof(uint32_t) + (2*MessageLenBits + 7) / 8;

    static Message& Merge(Message &aDest, Message &aSource) noexcept;

    Message() noexcept;
    Message(uint32_t aSeq, uint8_t *apData, size_t aLen) noexcept;
    Message(Buffer::Reader & aReader) noexcept;
    Message(Message&& aRhs) noexcept;
    Message(const Message& acRhs) noexcept;
    ~Message() noexcept;

    Message& operator=(Message&& aRhs) noexcept;
    Message& operator=(const Message& acRhs) noexcept;
    Message& operator+(Message& aRhs) noexcept;

    uint32_t GetSeq() const noexcept;
    size_t GetLen() const noexcept;
    bool IsComplete() const noexcept;
    bool IsValid() const noexcept;
    Buffer::Reader GetData() const noexcept;
    size_t Write(Buffer::Writer & aWriter, size_t aOffset=0) const noexcept;

private:

    class Slice : public AllocatorCompatible
    {
    public:
        Slice(size_t aOffset, size_t aLen) noexcept;
        Slice(uint8_t * apData, size_t aLen) noexcept;
        Slice(Buffer::Reader& aReader, size_t aMessageLength) noexcept;
        Slice(Slice&& aRhs) noexcept;
        Slice(const Slice& acRhs) noexcept;

        Slice& operator=(Slice&& aRhs) noexcept;
        Slice& operator=(const Slice& acRhs) noexcept;

        size_t GetEndOffset() const noexcept;

    private:
        size_t m_offset;
        size_t m_len;
        Buffer m_data;
        bool m_empty;

        friend class Message;
    };

    std::list<Slice> m_slices;
    size_t m_len;
    uint32_t m_seq;

};