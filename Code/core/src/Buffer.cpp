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