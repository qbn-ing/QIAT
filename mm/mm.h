#pragma once
#include "../include/types.h"
#include "../include/mmu.h"
#include "../include/sched.h"

#define VA(pa) ((void *)((uint64_t)(pa) + 0xffff888000000000))
#define PA(x) ((int64_t)(x) - 0xffff888000000000)
#define PA2(x) ((int64_t)(x) - 0xffffffff80000000)

extern uint64_t mem_size;
extern int8_t pages[MAX_PAGES];
struct e820entry
{
    uint64_t addr;
    uint64_t size;
    uint32_t type;

} __attribute__((packed)); // 不要进行对齐
struct e820map
{
    uint32_t nr_entry; // 个数
    struct e820entry map[E820_MAX];
};
uint64_t alloc_page();
void free_page(uint64_t addr);
void map_range(uint64_t pml4_pa, uint64_t from_va, uint64_t to_pa, int8_t us, int64_t npage);
void mm_init();
void *malloc(int32_t size);
void free(void *va);
int64_t sys_malloc_big0(int32_t size, int64_t plm4); // 分配大于4KB的
int64_t sys_malloc_big(int32_t size);                // 分配大于4KB的
