#pragma once

class Allocator
{
public:

    virtual void* Allocate(size_t aSize) = 0;
    virtual void Free(void* apData) = 0;
    virtual size_t Size(void* apData) = 0;

    template<class T, class... Args>
    T* New(Args... args)
    {
        void* pData = Allocate(sizeof(T));
        if (pData)
        {
            new (pData) T(std::forward<Args...>(args));
        }
    }

    template<class T>
    void Delete(T* apData)
    {
        if (apData == nullptr) return;

        apData->~T();
        Free(apData);
    }
};