#pragma once
// 定义段
#define SEG_KCODE 0x8
#define SEG_KDATA 0x10
#define SEG_U32CODE 0x1b
#define SEG_UDATA 0x23
#define SEG_UCODE 0x2b
#define SEG_U32DATA SEG_UDATA

#define OFFSET_16 0x10000
#define KERNEL16_ADDR 0x7e00
#define KERNEL16_SIZE 0x1000
#define KERNEL16_LOCAL 1
#define KERNEL32_ADDR 0x20000
#define KERNEL32_SIZE 1024
#define KERNEL32_LOCAL 3
#define KERNEL64_ADDR 0x100000
#define KERNEL64_SIZE 2048
#define KERNEL64_LOCAL 5

#define E820_MAX 128
#define E820InFo_ADDR 0x9000
#define E820_ADDR 0x9004
#define E820_RAM 1

#define VBEiInfo_ADDR 0x9200

#define PAGE_SIZE 4096
#define MAX_PAGES (1024 * 1024)
// 1M*4kb=4G
#define KERNEL_PAGES (1024 * 16)
// 为内核保留16K个页面
#define TASK0_PML4 0x10000
// 操作系统进程的四级页表基址

#define GDT_TSS_ENTRY 6
// 代码段和数据段用了前5项