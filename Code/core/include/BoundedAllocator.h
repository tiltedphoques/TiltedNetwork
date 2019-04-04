#pragma once

#include "StandardAllocator.h"

class BoundedAllocator : public StandardAllocator
{
public:

    BoundedAllocator(size_t aMaximumAllocationSize);

    virtual void* Allocate(size_t aSize) override;
    virtual void Free(void* apData) override;

private:

    size_t m_availableMemory;
};