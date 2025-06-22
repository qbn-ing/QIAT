#include "../include/readfile.h"
void head64(void)
{
    print('6');
    uint8_t *pa = (uint8_t *)KERNEL64_ADDR; // 内存的起始地址
    uint32_t count = 1024 * 100;            // 要读取的字节数
    uint32_t offset = KERNEL64_LOCAL;       // 内核文件在磁盘上的起始扇区
    readfile(pa, count, offset);
    print('4');
    // __asm__("hlt\n");
    __asm__ volatile("ljmpl $0x8, $0x100000\n");
}
