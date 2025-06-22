#include "../include/readfile.h"
#include "../include/mmu.h"
void bootloader(void)
{

    uint8_t *pa = (uint8_t *)(KERNEL16_ADDR);
    uint32_t count = 1024;
    uint32_t offset = KERNEL16_LOCAL;
    readfile(pa, count, offset);
    void (*entry)(void);
    entry = (void (*)(void))(0x7e00);
    entry();
}