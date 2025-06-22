#pragma once
#include "types.h"
#define NEXT_LEFT 0
#define NEXT_CENTER 1
#define NEXT_RIGHT 2
#define FOLLOW 3

#define print(x)                              \
  do                                          \
  {                                           \
    int size = sizeof(x);                     \
    if (size <= 4)                            \
    {                                         \
      __asm__ volatile("mov $0x3f8, %%dx\n\t" \
                       "out %%eax, %%dx\n\t"  \
                       :                      \
                       : "a"(x)               \
                       : "dx");               \
    }                                         \
    else if (size == 8)                       \
    {                                         \
      __asm__ volatile("mov $0x3f8, %%dx\n\t" \
                       "out %%eax, %%dx\n\t"  \
                       "shr $32, %%rax\n\t"   \
                       "out %%eax, %%dx\n\t"  \
                       :                      \
                       : "a"(x)               \
                       : "dx");               \
    }                                         \
  } while (0)

inline void outb(uint16_t port, uint8_t data)
{
  __asm__ volatile("out %0, %1" : : "a"(data), "d"(port));
}

inline uint8_t in(uint16_t port)
{
  uint8_t data;
  __asm__ volatile("in %1, %0" : "=a"(data) : "d"(port));
  return data;
}

static inline void insw(uint16_t port, void *addr, int32_t cnt)
{
  __asm__ volatile(
      "cld\n"                          // 清方向标志，确保递增
      "rep insw"                       // 重复前缀 使用 insw 指令，每次读取16位（2字节）
      : "=D"(addr), "=c"(cnt)          // 输出操作数，修改后的地址和计数器
      : "d"(port), "0"(addr), "1"(cnt) // 输入操作数，端口地址，原始地址和计数器
      : "memory", "cc"                 // 污染内存和条件码
  );
}
static inline void
insl(int port, void *addr, int cnt)
{
  __asm__ volatile("cld\n rep insl"
                   : "=D"(addr), "=c"(cnt)
                   : "d"(port), "0"(addr), "1"(cnt)
                   : "memory", "cc");
}
static inline void clear()
{
  __asm__ volatile(
      "movb $0x00, %ah\n"
      "movb $0x06, %al\n"
      "xorw %bx, %bx\n"
      "xorw %cx, %cx\n"
      "movw $0x184f, %di\n"
      "int $0x10\n");
}
static inline void get_cpuid(uint32_t Mop, uint32_t Sop, uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d)
{
  __asm__ volatile(
      "cpuid"
      : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
      : "a"(Mop), "c"(Sop));
}
inline void hlt()
{
  while (1)
  {
  };
  // __asm__ volatile("hlt");
}
static inline void printn()
{
  print('\n');
  print('\r');
}
static inline void hprint(uint64_t x)
{
  uint64_t y = x, tp = 1;
  y >>= 4;
  while (y)
  {
    tp <<= 4;
    y >>= 4;
  }

  print('0');
  print('x');
  while (tp)
  {
    uint8_t t = x / tp;
    if (t < 10)
      print('0' + t);
    else
      print('a' + t - 10);
    x %= tp;
    tp >>= 4;
  }
  printn();
}
static inline void dprint(int64_t x)
{
  // print('.');
  if (x < 0)
  {
    print('-');
    x = -x;
  }

  // print(',');
  int64_t y = x, tp = 1;
  // int64_t cnt = 0;
  y /= 10;
  // hprint(y);
  while (y)
  {
    // print('+');
    y /= 10;
    tp *= 10;
  }
  while (tp)
  {
    print('0' + x / tp);
    // outb(0x3f8, '0' + x / tp);
    x %= tp;
    tp /= 10;
  }
  printn();
}

static inline void sti()
{
  __asm__ volatile("sti");
}
static inline void cli()
{
  __asm__ volatile("cli");
}
static inline void lcr3(uint64_t cr3)
{
  __asm__ volatile("mov %0, %%cr3" : : "r"(cr3));
}