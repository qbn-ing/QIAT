#pragma once
#include "types.h"
#include "string.h"
#include "asm.h"

#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL
#define UPPER_MASK 0x80000000UL
#define LOWER_MASK 0x7fffffffUL

void set_seed(uint64_t s);
double powf(double x, int32_t y);
uint64_t rand(uint64_t mod);
#define max(x, y) ((x) > (y) ? (x) : (y))
#define min(x, y) ((x) > (y) ? (y) : (x))
#define SWAP(x, y, T) \
    do                \
    {                 \
        T SWAP = x;   \
        x = y;        \
        y = SWAP;     \
    } while (0)
