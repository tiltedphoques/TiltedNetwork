#include "Buffer.h"
#include <algorithm>


Buffer::Buffer()
    : m_pData(nullptr)
    , m_size(0)
{

}

Buffer::Buffer(size_t aSize)
    : m_pData(nullptr)
    , m_size(aSize)
{
    if(m_size > 0)
        m_pData = (uint8_t*)GetAllocator()->Allocate(m_size);
}

Buffer::Buffer(const Buffer& acBuffer)
    : Buffer(acBuffer.m_size)
{
    if(m_pData && acBuffer.GetData())
        std::copy(acBuffer.GetData(), acBuffer.GetData() + m_size, m_pData);
}

Buffer::Buffer(Buffer&& aBuffer)
    : m_pData(std::move(aBuffer.m_pData))
    , m_size(std::move(aBuffer.m_size))
{
    aBuffer.m_pData = nullptr;
    aBuffer.m_size = 0;
}

Buffer::~Buffer()
{
    GetAllocator()->Free(m_pData);
}

uint8_t Buffer::operator[](size_t aIndex) const
{
    return m_pData[aIndex];
}

uint8_t& Buffer::operator[](size_t aIndex)
{
    return m_pData[aIndex];
}

Buffer& Buffer::operator=(const Buffer& acBuffer)
{
    this->~Buffer();
    new (this) Buffer(acBuffer);

    return *this;
}

Buffer& Buffer::operator=(Buffer&& aBuffer)
{
    std::swap(aBuffer.m_pData, m_pData);
    std::swap(aBuffer.m_size, m_size);

    return *this;
}

size_t Buffer::GetSize() const
{
    return m_size;
}

const uint8_t* Buffer::GetData() const
{
    return m_pData;
}

uint8_t* Buffer::GetWriteData()
{
    return m_pData;
}

Buffer::Cursor::Cursor(Buffer* apBuffer)
    : m_pBuffer(apBuffer)
    , m_bitPosition(0)
{

}

void Buffer::Cursor::Advance(size_t aByteCount)
{
    m_bitPosition += aByteCount * 8;
}

void Buffer::Cursor::Reverse(size_t aByteCount)
{
    if (aByteCount * 8 <= m_bitPosition)
    {
        m_bitPosition -= aByteCount * 8;
    }
}

void Buffer::Cursor::Reset()
{
    m_bitPosition = 0;
}

bool Buffer::Cursor::Eof() const
{
    return m_bitPosition >= m_pBuffer->GetSize() * 8;
}

size_t Buffer::Cursor::GetBitPosition() const
{
    return m_bitPosition;
}

size_t Buffer::Cursor::GetBytePosition() const
{
    return m_bitPosition / 8;
}

Buffer::Reader::Reader(Buffer* apBuffer)
    : Buffer::Cursor(apBuffer)
{

}

bool Buffer::Reader::ReadBits(uint64_t& aDestination, size_t aCount)
{
    return false;
}

bool Buffer::Reader::ReadBytes(uint8_t* apDestination, size_t aCount)
{
    if (aCount + GetBytePosition() <= m_pBuffer->GetSize())
    {
        std::copy(m_pBuffer->GetData() + GetBytePosition(), m_pBuffer->GetData() + GetBytePosition() + aCount, apDestination);

        Advance(aCount);

        return true;
    }

    return false;
}

Buffer::Writer::Writer(Buffer* apBuffer)
    : Buffer::Cursor(apBuffer)
{

}

Buffer::Writer::~Writer()
{
}

bool Buffer::Writer::WriteBits(uint64_t aData, size_t aCount)
{
    return false;
}

bool Buffer::Writer::WriteBytes(uint8_t* apSource, size_t aCount)
{
    if (aCount + GetBytePosition() <= m_pBuffer->GetSize())
    {
        std::copy(apSource, apSource + aCount, m_pBuffer->GetWriteData() + GetBytePosition());

        Advance(aCount);

        return true;
    }

    return false;
}