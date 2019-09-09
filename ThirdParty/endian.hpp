#pragma once

// from https://stackoverflow.com/questions/1583791/constexpr-and-endianness
#define IS_LITTLE_ENDIAN  ('ABCD'==0x41424344UL) //41 42 43 44 = 'ABCD' hex ASCII code
#define IS_BIG_ENDIAN     ('ABCD'==0x44434241UL) //44 43 42 41 = 'DCBA' hex ASCII code
#define IS_UNKNOWN_ENDIAN (IS_LITTLE_ENDIAN == IS_BIG_ENDIAN)

// from http://stackoverflow.com/a/4956493/238609
template <typename T>
T swap_endian(T u)
{
    union
    {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (size_t k = 0; k < sizeof(T); k++)
        dest.u8[k] = source.u8[sizeof(T) - k - 1];

    return dest.u;
}

template <typename T>
T to_big_endian(T u)
{
#if IS_BIG_ENDIAN
    return u;
#else
    return swap_endian<T>(u);
#endif
}

template <typename T>
T to_little_endian(T u)
{
#if IS_LITTLE_ENDIAN
    return u;
#else
    return swap_endian<T>(u);
#endif
}
