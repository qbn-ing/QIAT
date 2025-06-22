#include "mm.h"
#include "../include/asm.h"
#include "../include/string.h"
struct bucket_desc // 记录该页面的内存块信息
{
    void *freeptr;            // 指向本页面内内存块
    struct bucket_desc *next; // 指向下一个页面的内存块描述
    int16_t refcnt;           // 页面中使用的块数
};
struct bucket_dir
{
    int16_t size;              // 该页面的内存块大小
    struct bucket_desc *bdesc; // 指向该页面的内存块描述
};
struct bucket_dir bucket_dir[] =
    {
        {16, NULL},
        {32, NULL},
        {64, NULL},
        {128, NULL},
        {256, NULL},
        {512, NULL},
        {1024, NULL},
        {2048, NULL},
        {0, NULL}}; // 不同大小的内存块的描述

void *malloc(int32_t size)
{
    void *va = NULL;
    struct bucket_dir *bdir = NULL;
    struct bucket_desc *bdesc = NULL;

    for (bdir = bucket_dir; bdir->size != 0; bdir++) // 找到能够容纳size大小的内存块的描述
    {
        if (bdir->size >= size)
        {
            break;
        }
    }

    if (bdir->size == 0) // 超出2048，无法分配
    {
        sprint("\n\rso big!\n\r");
        return NULL;
    }

    for (bdesc = bdir->bdesc; bdesc != NULL; bdesc = bdesc->next) // 找到一个空闲的内存块
    {
        if (bdesc->freeptr)
        {
            break;
        }
    }
    if (bdesc == NULL)
    {
        bdesc = VA(alloc_page());                                                // 分配一个页面
        bdesc->freeptr = (void *)((uint64_t)bdesc + sizeof(struct bucket_desc)); // 保留一个bucket_desc的空间
        bdesc->refcnt = 0;
        bdesc->next = NULL;

        uint64_t *p = bdesc->freeptr;
        int32_t i = 0;
        while (++i < ((PAGE_SIZE - sizeof(struct bucket_desc)) / bdir->size)) // 将整个页面划分成size大小的块
        {
            *p = (uint64_t)p + bdir->size; // 块的前8字节存储下一块的地址
            p = (uint64_t *)*p;
        }
        *p = 0;

        bdesc->next = bdir->bdesc; // 将新分配的页面加入到bdir的链表中
        bdir->bdesc = bdesc;
    }
    va = bdesc->freeptr;
    bdesc->freeptr = (void *)*(uint64_t *)bdesc->freeptr;
    bdesc->refcnt++;
    sprint("va==");
    hprint((int64_t)va);
    return va;
}

void free(void *va)
{
    uint64_t p = (uint64_t)va & 0xfffff000;
    struct bucket_dir *bdir = bucket_dir;
    struct bucket_desc *bdesc = NULL;

    for (; bdir->size != 0; bdir++)
    {
        bdesc = bdir->bdesc;
        for (; bdesc != NULL; bdesc = bdesc->next)
        {
            if ((uint64_t)bdesc == p) // 找到va所在的页面
            {
                *((uint64_t *)va) = (uint64_t)bdesc->freeptr; // 此次释放的内存块的前8字节存储下一块的地址
                bdesc->freeptr = va;                          // 第一个空闲块成了此处
                bdesc->refcnt--;                              // 已分配的块--
                break;
            }
        }
    }
    if (bdesc && bdesc->refcnt == 0) // 如果该页面没有被使用，释放该页面
    {
        struct bucket_desc *prev = NULL;
        struct bucket_desc *tmp = bdir->bdesc;
        for (; tmp; tmp = tmp->next) // 从bdir的链表中删除该页面
        {
            if ((uint64_t)tmp == (uint64_t)bdesc)
            {
                break;
            }
            prev = tmp;
        }
        if (prev != NULL) // 如果是第一个
        {
            bdir->bdesc = bdesc->next;
        }
        else // 如果不是第一个页面
        {
            prev->next = bdesc->next;
        }
        free_page((uint64_t)bdesc);
    }
}

#define VAM(pa) ((void *)((uint64_t)(pa)))          //+ 0xfffff00000000000)))
int64_t sys_malloc_big0(int32_t size, int64_t plm4) // 分配大于4KB的
{
    int32_t i = 1;
    int64_t tmp = alloc_page(); // 先申请一个页
    volatile int64_t va = (int64_t)VAM(tmp);
    map_range(plm4, va, tmp, 0x4, 1);
    while (i * PAGE_SIZE < size)
    {
        va += PAGE_SIZE;
        map_range(plm4, va, alloc_page(), 0x4, 1);
        i++;
    }
    return (int64_t)VAM(tmp);
}
int64_t sys_malloc_big(int32_t size) // 分配内存，最少一个页,作为系统调用吧
{
    return sys_malloc_big0(size, current->plm4);
}
