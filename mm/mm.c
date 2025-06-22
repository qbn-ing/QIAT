#include "mm.h"
#include "../include/asm.h"
#include "../include/string.h"
#include "../include/chara.h"
int8_t pages[MAX_PAGES];
uint64_t mem_size = 0;

uint64_t alloc_page() // 分配一个页面并返回物理地址
{
    uint64_t addr = 0;
    for (volatile int64_t i = KERNEL_PAGES; i < mem_size / PAGE_SIZE; i++)
    {
        if (pages[i] == 0)
        {
            pages[i] = 1;
            addr = i * PAGE_SIZE;
            break;
        }
    }
    return addr;
}
void free_page(uint64_t addr) // 释放一个页面
{
    if (addr > (uint64_t)VA(0)) // 如果是虚拟地址，转换成物理地址
        addr = PA(addr);
    int64_t index = addr / PAGE_SIZE;
    pages[index] = 0;
}
// 完成以页面为粒度的虚拟地址到物理地址的映射
// pml4_pa 根页表的物理地址 from_va 需要映射的起始虚拟地址  to_pa 对应的物理地址 us 访问权限(0x4是仅允许内核访问) npage 映射的页面数
void map_range(uint64_t pml4_pa, uint64_t from_va, uint64_t to_pa, int8_t us, int64_t npage)
{
    volatile int64_t n = 0;
    while (n < npage)
    {
        // pml4 40-48位
        uint64_t *page_va = VA(pml4_pa);         // 找到页表基址的虚拟地址
        int16_t index = (from_va >> 39) & 0x1ff; // 仅保留虚拟地址当前的9位
        uint64_t entry = *(page_va + index);     //*(page_va + index); // 基址加索引得到对应的页表项

        if (!entry & 0x1) // 页表项不存在
        {
            *(page_va + index) = alloc_page() | 0x3 | us; // 分配一个页面并设置访问权限
            entry = *(page_va + index);                   // 在分配的页面那里建立后面的页表
        }
        // pml3 31-39位
        page_va = VA(entry & 0xffffffffff000);
        index = (from_va >> 30) & 0x1ff;
        entry = *(page_va + index);
        if (!(entry & 0x1))
        {
            *(page_va + index) = alloc_page() | 0x3 | us;
            entry = *(page_va + index);
        }
        // pml2 22-30位
        page_va = VA(entry & 0xffffffffff000);
        index = (from_va >> 21) & 0x1ff;
        entry = *(page_va + index);
        if (!(entry & 0x1))
        {
            *(page_va + index) = alloc_page() | 0x3 | us;
            entry = *(page_va + index);
        }
        // pml1 13-21位
        page_va = VA(entry & 0xffffffffff000);
        index = (from_va >> 12) & 0x1ff;
        entry = *(page_va + index);
        if (!(entry & 0x1)) // 页表项不存在
        {
            *(page_va + index) = (to_pa + PAGE_SIZE * n) | 0x3 | us; // 当前页表项的物理地址
            // hprint((int64_t)((to_pa + PAGE_SIZE * n) | 0x3 | us));
        }
        n++; // 映射完一个页面
        from_va += PAGE_SIZE;
        // sprint("page ok!\n\r");
    }
    // sprint("mapOk\n\r");
}
void mm_init()
{
    struct e820map *e820 = (struct e820map *)0x9000;

    for (int32_t i = 0; i < e820->nr_entry; i++)
    {
        if (e820->map[i].type == E820_RAM)
        {
            uint64_t tmp = e820->map[i].addr + e820->map[i].size;
            if (tmp > mem_size)
            {
                mem_size = tmp;
            }
        }
    }
    // sprint("mem_size:\n\r");
    // dprint(mem_size);
    // printn();
    // hprint(mem_size);
    // 初始化内存池
    memset(pages, 0, MAX_PAGES);
    for (volatile int32_t i = 0; i < KERNEL_PAGES; i++)
    {
        pages[i] = 1;
    }
    // 建立物理内存的映射,物理内存向上取整
    map_range(TASK0_PML4, (uint64_t)VA(0), 0, 0, (mem_size + PAGE_SIZE - 1) / PAGE_SIZE);
}