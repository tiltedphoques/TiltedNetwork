#include "BoundedAllocator.h"


BoundedAllocator::BoundedAllocator(size_t aMaximumAllocationSize)
    : m_availableMemory{aMaximumAllocationSize}
{

}

void* BoundedAllocator::Allocate(size_t aSize)
{
    if (m_availableMemory >= aSize)
    {
        m_availableMemory -= aSize;
        return StandardAllocator::Allocate(aSize);
    }

    return nullptr;
}

void BoundedAllocator::Free(void* apData)
{
    m_availableMemory += Size(apData);

    StandardAllocator::Free(apData);
}
