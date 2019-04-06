#pragma once

#include "Allocator.h"

class ScratchAllocator : public Allocator
{
public:

    ScratchAllocator(size_t aSize);
    virtual ~ScratchAllocator();

    virtual void* Allocate(size_t aSize) override;
    virtual void Free(void* apData) override;
    virtual size_t Size(void* apData) override;

private:

    size_t m_size;
    void* m_pData;
    void* m_pBaseData;
};