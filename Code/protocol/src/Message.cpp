#include "Message.h"

Message::Message(uint32_t aSeq, uint8_t *apData, size_t aLen) :
    m_seq(aSeq)
    , m_len(aLen)
{
    m_data = Buffer(aLen);
    std::copy(apData, apData+aLen, m_data.GetWriteData());
}

Message::~Message()
{
    
}

Message::Message(Buffer::Reader& aReader) :
    m_seq(0)
    , m_len(SIZE_MAX)
{
    aReader.ReadBytes((uint8_t *) &m_seq, sizeof(m_seq));
    aReader.ReadBytes((uint8_t *)&m_len, sizeof(m_len));

    if (m_len <= aReader.GetSize() - aReader.GetBytePosition())
    {
        m_data = Buffer(m_len);
        aReader.ReadBytes(m_data.GetWriteData(), m_len);
    }
    else
    {
        m_data = Buffer();
    }
}

uint32_t Message::GetSeq() const noexcept
{
    return m_seq;
}

Buffer::Reader Message::GetData() noexcept
{
    return Buffer::Reader(&m_data);
}

bool Message::Write(Buffer::Writer& aWriter) const noexcept
{
    return aWriter.WriteBytes((uint8_t *) &m_seq, sizeof(m_seq))
    && aWriter.WriteBytes((uint8_t *)&m_len, sizeof(m_len))
    && aWriter.WriteBytes(m_data.GetData(), m_len);
}

