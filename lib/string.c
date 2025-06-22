// #pragma once
#include "../include/string.h"
#include "../include/asm.h"
inline void *memset(void *s, int8_t c, uint32_t n)
{
    int8_t *tmp = (int8_t *)s;
    while (n--)
    {
        *tmp++ = c;
    }
    return s;
}
inline int32_t strlen(int8_t *s)
{
    uint32_t len = 0;
    while (s[len] != '\0')
        len++;
    return len;
}
inline void sprint(int8_t *str)
{
    uint32_t len = strlen(str);
    for (uint32_t i = 0; i < len; i++)
    {
        print(str[i]);
    }
}
void memcpy(void *dest, void *src, uint32_t n) // 按字节复制，n是字节数
{
    uint8_t *d = (uint8_t *)dest;
    uint8_t *s = (uint8_t *)src;
    for (uint32_t i = 0; i < n; i++)
    {
        d[i] = s[i];
    }
}
int8_t *strcpy(int8_t *s, const int8_t *t, int32_t n)
{
    int8_t *os;

    os = s;
    if (n <= 0)
        return os;
    while (--n > 0 && (*s++ = *t++) != 0)
        ;
    *s = 0;
    return os;
}
int8_t toLower(int8_t c)
{
    if (c >= 'A' && c <= 'Z')
        return c + 32;
    return c;
}
int8_t toUpper(int8_t c)
{
    if (c >= 'a' && c <= 'z')
        return c - 32;
    return c;
}