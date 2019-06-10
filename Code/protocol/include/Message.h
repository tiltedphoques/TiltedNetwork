#pragma once

#include "Buffer.h"
#include "Allocator.h"
#include "Outcome.h"

class Message: public AllocatorCompatible
{
public:
    Message(uint32_t aSeq, uint8_t *apData, size_t aLen);
    Message(Buffer::Reader & aReader);
    ~Message();

    uint32_t GetSeq() const noexcept;
    Buffer::Reader GetData() noexcept;
    bool Write(Buffer::Writer & aWriter) const noexcept;

private:

    uint32_t m_seq;
    size_t m_len;
    Buffer m_data;
};