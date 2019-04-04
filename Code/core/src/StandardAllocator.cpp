#include "StandardAllocator.h"

#include <cstdlib>

void* StandardAllocator::Allocate(size_t aSize)
{
    return malloc(aSize);
}

void StandardAllocator::Free(void* apData)
{
    free(apData);
}

size_t StandardAllocator::Size(void* apData)
{
#ifdef _WIN32
    return _msize(apData);
#else
    static_assert(false, "Not implemented");
    return 0;
#endif
}