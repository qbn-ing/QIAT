#include "types.h"
#include "asm.h"
#include "mmu.h"

#define SECTSIZE 512

void waitdisk(void)
{
    // 从0x1f7读取硬盘控制器状态，等待硬盘就绪
    while ((in(0x1F7) & 0xC0) != 0x40)
        ;
}
void readsect(void *dst, uint32_t offset) // 从offset读一个扇区到dst地址
{
    // 等待硬盘就绪
    waitdisk();
    // 发送命令
    outb(0x1F2, 0x1);                   // 读一个扇区
    outb(0x1F3, offset);                // 低8位
    outb(0x1F4, offset >> 8);           // 次低八位
    outb(0x1F5, offset >> 16);          // 次高八位
    outb(0x1F6, (offset >> 24) | 0xE0); // 高八位，高四位是控制位并设置为LBA模式
    outb(0x1F7, 0x20);                  // 送读扇区命令
    // 等待硬盘就绪
    waitdisk();
    // 读取数据
    insl(0x1F0, dst, SECTSIZE / 4);
}
void readfile(uint8_t *pa, int32_t count, int32_t offset) // 从offset读count字节的数据到pa
{
    volatile uint8_t *epa = pa + count;
    for (; pa < epa; pa += SECTSIZE, offset++)
    {
        readsect(pa, offset);
    }
}
