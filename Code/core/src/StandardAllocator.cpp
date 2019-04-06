#include "StandardAllocator.h"

#include <cstdlib>
#include <malloc.h>

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
    if (apData == nullptr) return 0;

#ifdef _WIN32
    return _msize(apData);
#elif __linux__
    return malloc_usable_size(apData);
#else
    static_assert(false, "Not implemented");
    return 0;
#endif
}