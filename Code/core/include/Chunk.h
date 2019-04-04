#pragma once

#include <cstdint>

class Chunk
{
public:

    Chunk();
    ~Chunk();

private:

    size_t m_size;
    uint8_t* m_pBuffer;
};