#pragma once

#include "Allocator.h"

class StandardAllocator : public Allocator
{
public:

    virtual void* Allocate(size_t aSize) override;
    virtual void Free(void* apData) override;
    virtual size_t Size(void* apData) override;
};