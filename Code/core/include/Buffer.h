#pragma once

#include <cstdint>
#include "Allocator.h"

class Buffer : public AllocatorCompatible
{
public:

    Buffer(size_t aSize);
    virtual ~Buffer();

    uint8_t operator[](size_t aIndex) const;
    uint8_t& operator[](size_t aIndex);

    size_t GetSize() const;

    const uint8_t* GetData() const;
    uint8_t* GetData();

private:

    uint8_t* m_pData;
    size_t m_size;
};