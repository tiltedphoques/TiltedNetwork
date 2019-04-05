#pragma once

#include <cstdint>

class Buffer
{
public:

    Buffer();
    virtual ~Buffer() = 0;

private:
};

class FixedBuffer
{
public:

    FixedBuffer(size_t aLength);
    virtual ~FixedBuffer();

private:

    Allocator* m_pAllocator;
};