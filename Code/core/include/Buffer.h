#pragma once

#include <cstdint>
#include "Allocator.h"

class Buffer : public AllocatorCompatible
{
public:

    Buffer();
    Buffer(size_t aSize);
    Buffer(const Buffer& acBuffer);
    Buffer(Buffer&& aBuffer) noexcept;
    virtual ~Buffer();

    uint8_t operator[](size_t aIndex) const;
    uint8_t& operator[](size_t aIndex);

    Buffer& operator=(const Buffer& acBuffer);
    Buffer& operator=(Buffer&& aBuffer) noexcept;

    size_t GetSize() const;

    const uint8_t* GetData() const;
    uint8_t* GetWriteData();

    struct Cursor
    {
        Cursor(Buffer* apBuffer);

        void Reset();
        bool Eof() const;
        void Advance(size_t aByteCount);
        void Reverse(size_t aByteCount);

        size_t GetBytePosition() const;
        size_t GetBitPosition() const;
        size_t GetSize() const;

    protected:

        size_t m_bitPosition;
        Buffer* m_pBuffer;
    };

    struct Reader : public Cursor
    {
        Reader(Buffer* apBuffer);

        bool ReadBits(uint64_t& aDestination, size_t aCount);
        bool ReadBytes(uint8_t* apDestination, size_t aCount);
    };

    struct Writer : public Cursor
    {
        Writer(Buffer* apBuffer);
        ~Writer();

        bool WriteBits(uint64_t aData, size_t aCount);
        bool WriteBytes(const uint8_t* apSource, size_t aCount);
    };

private:

    uint8_t* m_pData;
    size_t m_size;
};
