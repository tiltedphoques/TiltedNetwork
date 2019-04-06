#pragma once

#include <cstdint>
#include "Allocator.h"

class Buffer : public AllocatorCompatible
{
public:

    Buffer();
    Buffer(size_t aSize);
    Buffer(const Buffer& acBuffer);
    Buffer(Buffer&& aBuffer);
    virtual ~Buffer();

    uint8_t operator[](size_t aIndex) const;
    uint8_t& operator[](size_t aIndex);

    Buffer& operator=(const Buffer& acBuffer);
    Buffer& operator=(Buffer&& aBuffer);

    size_t GetSize() const;

    const uint8_t* GetData() const;
    uint8_t* GetWriteData();

private:

    uint8_t* m_pData;
    size_t m_size;
};