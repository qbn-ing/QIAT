#pragma once
#include "types.h"
#include "mmu.h"
#include "sched.h"

#define IO_BITMAP_BYTES (65536 / 8)

struct tss
{
    uint32_t reserved0;
    uint64_t rsp0; // 权限位0的栈指针
    uint64_t rsp1; // 权限位1的栈指针
    uint64_t rsp2; // 权限位2的栈指针
    uint32_t reserved1;
    uint64_t ist[7]; // 中断栈表
    uint32_t reserved2;
    uint32_t reserved3;
    uint16_t reserved4;
    uint16_t iobp;                      // IO权限位图基址
    uint8_t io_bitmap[IO_BITMAP_BYTES]; // IO权限位图
} __attribute__((packed));

struct tss_desc
{
    uint16_t limit0;                                               // 任务段长度
    uint16_t base0;                                                // 任务段基址
    uint16_t base1 : 8, type : 4, desc_type : 1, dpl : 2, p : 1;   // 段类型 段描述符类型 权限 存在位
    uint16_t limit1 : 4, avl : 1, reserved0 : 2, g : 1, base2 : 8; // 系统使用位 段粒度
    uint32_t base3;
    uint32_t reserved1;
} __attribute__((packed));
void tss_init(); // 初始化tss
extern struct tss tss;