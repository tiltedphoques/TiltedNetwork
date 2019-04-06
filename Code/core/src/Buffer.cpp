#include "Buffer.h"


Buffer::Buffer(size_t aSize)
    : m_size(aSize)
{
    m_pData = (uint8_t*)GetAllocator()->Allocate(m_size);
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

size_t Buffer::GetSize() const
{
    return m_size;
}

const uint8_t* Buffer::GetData() const
{
    return m_pData;
}

uint8_t* Buffer::GetData()
{
    return m_pData;
}