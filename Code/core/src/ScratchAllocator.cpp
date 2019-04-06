#include "ScratchAllocator.h"
#include <memory>


ScratchAllocator::ScratchAllocator(size_t aSize)
    : m_size(aSize)
{
    m_pBaseData = m_pData = Allocator::GetDefault()->Allocate(aSize);

    if (m_pData == nullptr)
    {
        m_size = 0;
    }
}

ScratchAllocator::~ScratchAllocator()
{
    Allocator::GetDefault()->Free(m_pBaseData);
}

void* ScratchAllocator::Allocate(size_t aSize)
{
    if (std::align(alignof(std::max_align_t), aSize, m_pData, m_size))
    {
        void* pResult = m_pData;
        m_pData = (char*)m_pData + aSize;
        m_size -= aSize;
        return pResult;
    }

    return nullptr;
}

void ScratchAllocator::Free(void* apData)
{
    // do nothing here
    (void)apData;
}

size_t ScratchAllocator::Size(void* apData)
{
    (void)apData;
    return m_size;
}