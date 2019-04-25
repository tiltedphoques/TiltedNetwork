#pragma once

#include "Buffer.h"
#include "Allocator.h"
#include <array>

class ChachaFilter : public AllocatorCompatible
{
public:

    ChachaFilter();
    ~ChachaFilter();

    void ProcessTryConnect(Buffer::Writer* apBuffer);
    void ProcessConnect(Buffer::Reader* apBuffer);

    void ProcessSend(Buffer::Writer* apBuffer, uint32_t aSequenceNumber);
    void ProcessReceive(Buffer::Reader* apBuffer, uint32_t aSequenceNumber);

private:

    void* m_pPimpl;
    std::array<uint8_t, 20> m_iv;
};