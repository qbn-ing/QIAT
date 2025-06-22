#pragma once
#include "types.h"

void *memset(void *s, int8_t c, uint32_t n);
// uint8_t *toStr(uint64_t x, uint8_t types, uint8_t *buffer, uint32_t buffer_size); // 这里的buffer*需要实现内存管理
int32_t strlen(int8_t *s);
void sprint(int8_t *str);
void memcpy(void *dest, void *src, uint32_t n);
int8_t *strcpy(int8_t *s, const int8_t *t, int32_t n);
int8_t toLower(int8_t c);
int8_t toUpper(int8_t c);