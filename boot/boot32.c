// 加载32位引导内核
#include "../include/readfile.h"
#include "../include/mmu.h"
// __asm__(".code16\n"); // 用了这个后无法正常读取文件
void boot32(void)
{
    uint8_t *pa = (uint8_t *)(0xA000);
    int32_t count = 512 * 2;
    int32_t offset = KERNEL32_LOCAL;
    readfile(pa, count, offset);
    // outb(0x3f8, 'C');
    __asm__ volatile("ljmpl $0x8,$0xa000");
}