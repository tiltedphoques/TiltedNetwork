#pragma once

#include "Allocator.h"

template<class T>
class TrackAllocator : public Allocator
{
public:

    virtual ~TrackAllocator() {}

    virtual void* Allocate(size_t aSize) override
    {
        void* pData = m_allocator.Allocate(aSize);

        if (pData)
            m_usedMemory += m_allocator.Size(pData);

        return pData;
    }

    virtual void Free(void* apData) override
    {
        m_usedMemory -= m_allocator.Size(apData);
    }

    virtual size_t Size(void* apData) override
    {
        return m_allocator.Size(apData);
    }

    size_t GetUsedMemory() const
    {
        return m_usedMemory;
    }

private:

    T m_allocator;
    size_t m_usedMemory{ 0 };
};