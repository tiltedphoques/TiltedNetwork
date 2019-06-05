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

Buffer::Buffer(Buffer&& aBuffer) noexcept
{
    SetAllocator(aBuffer.GetAllocator());

    m_pData = aBuffer.m_pData;
    m_size = aBuffer.m_size;

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

Buffer& Buffer::operator=(Buffer&& aBuffer) noexcept
{
    std::swap(aBuffer.m_pData, m_pData);
    std::swap(aBuffer.m_size, m_size);

    // Swap allocators
    auto pAllocator = GetAllocator();
    SetAllocator(aBuffer.GetAllocator());
    aBuffer.SetAllocator(pAllocator);

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
    : m_bitPosition(0)
    , m_pBuffer(apBuffer)
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

size_t Buffer::Cursor::GetSize() const
{
    return m_pBuffer->GetSize();
}

Buffer::Reader::Reader(Buffer* apBuffer)
    : Buffer::Cursor(apBuffer)
{

}

bool Buffer::Reader::ReadBits(uint64_t& aDestination, size_t aCount)
{
    aDestination = 0;

    // Number of bits to read in the current byte
    auto bitIndex = m_bitPosition & 0x7;
    size_t bitsToRead = 0;

    auto countOffset = aCount + bitIndex;
    // Compute how many bytes we will end up reading
    auto bytesToRead = ((countOffset & ~0x7) + ((countOffset & 0x7) != 0 ? 8 : 0)) >> 3;
    if (bytesToRead + GetBytePosition() > m_pBuffer->GetSize())
    {
        return false;
    }

    uint64_t endBits = 0;

    auto* pLocation = m_pBuffer->GetData() + GetBytePosition();

    if (bitIndex != 0)
    {
        bitsToRead = 8 - bitIndex;

        // If we are requesting less bits that what is available
        bitsToRead = bitsToRead > aCount ? aCount : bitsToRead;

        endBits = ((*pLocation) >> bitIndex) & ((1 << bitsToRead) - 1);

        pLocation++;
        bytesToRead--;
    }
    else
        bitIndex = 0;

    std::copy(pLocation, pLocation + bytesToRead, (uint8_t*)& aDestination);
    aDestination <<= bitsToRead;
    aDestination |= endBits;
    aDestination &= ((uint64_t(1) << aCount) - 1);

    m_bitPosition += aCount;

    return true;
}

bool Buffer::Reader::ReadBytes(uint8_t* apDestination, size_t aCount)
{
    // Fix m_bitPosition to be at the start of the next full byte
    m_bitPosition = (m_bitPosition & ~0x7) + ((m_bitPosition & 0x7) != 0 ? 8 : 0);

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
    // Number of bits to write in the current byte
    auto bitIndex = m_bitPosition & 0x7;
    size_t bitsToWrite = 0;

    auto countOffset = aCount + bitIndex;
    // Compute how many bytes we will end up writing
    auto bytesToWrite = ((countOffset & ~0x7) + ((countOffset & 0x7) != 0 ? 8 : 0)) >> 3;

    if (bytesToWrite + GetBytePosition() > m_pBuffer->GetSize())
    {
        return false;
    }

    auto* pLocation = m_pBuffer->GetWriteData() + GetBytePosition();

    if (bitIndex != 0)
    {
        bitsToWrite = 8 - bitIndex;

        // If we are requesting less bits that what is available
        bitsToWrite = bitsToWrite > aCount ? aCount : bitsToWrite;

        // Select the data we want to keep
        auto workByte = *pLocation;
        auto workByteMask = ((1 << bitIndex) - 1);
        workByte &= workByteMask;

        // Write the part we want
        *pLocation = ((uint8_t)(aData & 0xFF) & ((1 << bitsToWrite) - 1)) << bitIndex;
        *pLocation |= workByte;

        pLocation++;
        bytesToWrite--;

        aData >>= bitsToWrite;
    }
    else
        bitIndex = 0;

    uint8_t* pDirectAccess = (uint8_t*)&aData;
    std::copy(pDirectAccess, pDirectAccess + bytesToWrite, pLocation);

    m_bitPosition += aCount;

    return true;
}

bool Buffer::Writer::WriteBytes(const uint8_t* apSource, size_t aCount)
{
    // Fix m_bitPosition to be at the start of the next full byte
    m_bitPosition = (m_bitPosition & ~0x7) + ((m_bitPosition & 0x7) != 0 ? 8 : 0);

    if (aCount + GetBytePosition() <= m_pBuffer->GetSize())
    {
        std::copy(apSource, apSource + aCount, m_pBuffer->GetWriteData() + GetBytePosition());

        Advance(aCount);

        return true;
    }

    return false;
}