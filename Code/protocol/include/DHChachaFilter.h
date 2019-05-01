#pragma once

#include "Buffer.h"
#include "Allocator.h"
#include <array>


struct DHChachaFilterPimpl;
class DHChachaFilter : public AllocatorCompatible
{
public:

    DHChachaFilter();
    ~DHChachaFilter();

    bool PreConnect(Buffer::Writer* apBuffer);
    bool ReceiveConnect(Buffer::Reader* apBuffer);
    
    // Called before the packet gets sent
    bool PreSend(Buffer::Writer* apBuffer, uint32_t aSequenceNumber);
    // Called after the payload is generated
    bool PostSend(uint8_t* apPayload, size_t aLength, uint32_t aSequenceNumber);
    // Called with the raw payload
    bool PreReceive(uint8_t* apPayload, size_t aLength, uint32_t aSequenceNumber);

private:

    void GenerateKeys();

    DHChachaFilterPimpl* m_pPimpl;
    std::array<uint8_t, 20> m_iv;
};