#include "Allocator.h"
#include "StandardAllocator.h"


thread_local Allocator* Allocator::s_allocatorStack[kMaxAllocatorCount];
thread_local int Allocator::s_currentAllocator{ -1 };


void Allocator::Push(Allocator* apAllocator)
{
    if (s_currentAllocator + 1 < kMaxAllocatorCount)
    {
        s_currentAllocator++;
        s_allocatorStack[s_currentAllocator] = apAllocator;
    }
}

Allocator* Allocator::Pop()
{
    if (s_currentAllocator >= 0)
    {
        auto pAllocator = s_allocatorStack[s_currentAllocator];
        s_currentAllocator--;

        return pAllocator;
    }

    return nullptr;
}

Allocator* Allocator::Get()
{
    if (s_currentAllocator >= 0)
    {
        return s_allocatorStack[s_currentAllocator];
    }

    return GetDefault();
}

Allocator* Allocator::GetDefault()
{
    static StandardAllocator s_allocator;
    return &s_allocator;
}

ScopedAllocator::ScopedAllocator(Allocator* apAllocator)
    : m_pAllocator(apAllocator)
{
    Allocator::Push(m_pAllocator);
}

ScopedAllocator::~ScopedAllocator()
{
    Allocator::Pop();
}