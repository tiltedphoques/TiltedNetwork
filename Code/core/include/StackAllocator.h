#pragma once

#include "Allocator.h"
#include <memory>

template <size_t Bytes>
class StackAllocator : public Allocator
{
public:

    StackAllocator();
    virtual ~StackAllocator();

    virtual void* Allocate(size_t aSize) override;
    virtual void Free(void* apData) override;
    virtual size_t Size(void* apData) override;

private:

    size_t m_size;
    void* m_pCursor;
    char m_data[Bytes];
};

template <size_t Bytes>
StackAllocator<Bytes>::StackAllocator()
    : m_size(Bytes)
{
    m_pCursor = (void*)m_data;
}

template <size_t Bytes>
StackAllocator<Bytes>::~StackAllocator()
{
}

template <size_t Bytes>
void* StackAllocator<Bytes>::Allocate(size_t aSize)
{
    if (std::align(alignof(std::max_align_t), aSize, m_pCursor, m_size))
    {
        void* pResult = m_pCursor;
        m_pCursor = (char*)m_pCursor + aSize;
        m_size -= aSize;
        return pResult;
    }

    return nullptr;
}

template <size_t Bytes>
void StackAllocator<Bytes>::Free(void* apData)
{
    // do nothing here
    (void)apData;
}

template <size_t Bytes>
size_t StackAllocator<Bytes>::Size(void* apData)
{
    (void)apData;
    return Bytes;
}