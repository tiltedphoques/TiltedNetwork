#include "Message.h"

#include <algorithm>

Message::Message(uint32_t aSeq, uint8_t *apData, size_t aLen) noexcept
    : m_slices({std::move(Message::Slice(apData, aLen))})
    , m_len(aLen)
    , m_seq(aSeq)
{

}

Message::Message(Buffer::Reader& aReader) noexcept
    : m_slices()
    , m_len(0)
    , m_seq(0)
{
    aReader.ReadBytes((uint8_t *)&m_seq, sizeof(m_seq));

    if (aReader.ReadBits(m_len, Message::MessageLenBits) && m_len <= Message::MaxMessageSize)
    {
        m_slices.push_front(Slice(aReader, m_len));
        
        Slice& slice = m_slices.front();
        
        if (slice.m_empty)
        {
            // Could not read slice for some reason. This message should be invalid
            m_len = 0;
        }
        else
        {
            if (slice.m_offset > 0)
            {
                // add empty slice before
                m_slices.push_front(Slice((size_t) 0, slice.m_offset));
            }

            if (slice.m_offset + slice.m_len < m_len)
            {
                // add empty slice after
                m_slices.push_back(Slice(slice.m_offset + slice.m_len, m_len - slice.m_offset - slice.m_len));
            }
        }
    }
}

Message::Message(Message && aRhs) noexcept
{
    this->operator=(std::move(aRhs));
}

Message::Message(const Message & acRhs) noexcept
{
    this->operator=(acRhs);
}

Message::~Message() noexcept
{
    
}

Message & Message::operator=(Message && aRhs) noexcept
{
    this->m_seq = aRhs.m_seq;
    this->m_len = aRhs.m_len;
    this->m_slices = std::move(aRhs.m_slices);

    aRhs.m_seq = 0;
    aRhs.m_len = 0;

    return *this;
}

Message & Message::operator=(const Message & acRhs) noexcept
{
    this->m_seq = acRhs.m_seq;
    this->m_len = acRhs.m_len;
    this->m_slices = acRhs.m_slices;

    return *this;
}

uint32_t Message::GetSeq() const noexcept
{
    return m_seq;
}

size_t Message::GetLen() const noexcept
{
    return m_len;
}

// Returns true if this message is ready (has only one slice)
bool Message::IsComplete() const noexcept
{
    return IsValid() && m_slices.size() == 1;
}

bool Message::IsValid() const noexcept
{
    return m_len > 0 && m_slices.size() > 0;
}

// Do not call this if IsComplete() returns false or face undefined behavior
Buffer::Reader Message::GetData() const noexcept
{
    return Buffer::Reader((Buffer *)&m_slices.front().m_data);
}

// Returns the number of bytes of real data (not headers) written
size_t Message::Write(Buffer::Writer& aWriter, size_t aOffset) const noexcept
{
    if (!IsComplete())
        return 0;   // this should never happen(tm)

    size_t headerBytes = sizeof(m_seq) + (2*Message::MessageLenBits + 7) / 8;
    size_t availableBytes = aWriter.GetSize() - aWriter.GetBytePosition();

    if (availableBytes <= headerBytes)
    {
        // Don't even try to write a slice because it won't fit
        return 0;
    }


    aWriter.WriteBytes((uint8_t *)&m_seq, sizeof(m_seq));
    aWriter.WriteBits(m_len, Message::MessageLenBits);
    aWriter.WriteBits(aOffset, Message::MessageLenBits);

    availableBytes = aWriter.GetSize() - aWriter.GetBytePosition();
    size_t bytesToWrite = std::min(availableBytes, m_len - aOffset);
    aWriter.WriteBytes(m_slices.front().m_data.GetData()+aOffset, bytesToWrite);
    
    return bytesToWrite;
}

Message::Slice::Slice(size_t aOffset, size_t aLen) noexcept
    : m_offset(aOffset)
    , m_len(aLen)
    , m_empty(true)
{

}

Message::Slice::Slice(uint8_t *apData, size_t aLen) noexcept
    : m_offset(0)
    , m_len(aLen)
    , m_empty(false)
{
    m_data = Buffer(aLen);
    std::copy(apData, apData + aLen, m_data.GetWriteData());
}

Message::Slice::Slice(Buffer::Reader& aReader, size_t aMessageLength) noexcept
    : m_offset(0)
    , m_len(0)
    , m_empty(true)
{
    if (aReader.ReadBits(m_offset, Message::MessageLenBits) && m_offset < aMessageLength)
    {
        m_len = std::min(aReader.GetSize() - aReader.GetBytePosition(), aMessageLength - m_offset);
        m_data = Buffer(m_len);
        m_empty = !aReader.ReadBytes(m_data.GetWriteData(), m_len);
    }
}

Message::Slice::Slice(Slice && aRhs) noexcept
{
    this->operator=(std::move(aRhs));
}

Message::Slice::Slice(const Slice & acRhs) noexcept
{
    this->operator=(acRhs);
}

Message::Slice& Message::Slice::operator=(Slice && aRhs) noexcept
{
    this->m_offset = aRhs.m_offset;
    this->m_len = aRhs.m_len;
    this->m_empty = aRhs.m_empty;
    this->m_data = std::move(aRhs.m_data);

    aRhs.m_offset = 0;
    aRhs.m_len = 0;
    aRhs.m_empty = true;

    return *this;
}

Message::Slice& Message::Slice::operator=(const Slice & acRhs) noexcept
{
    this->m_offset = acRhs.m_offset;
    this->m_len = acRhs.m_len;
    this->m_empty = acRhs.m_empty;
    this->m_data = acRhs.m_data;

    return *this;
}