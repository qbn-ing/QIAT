#include "vesa.h"
uint64_t vbf;
void vga_init()
{
    uint64_t *xz = VA(0x9200);
    uint32_t *vbef = (uint32_t *)VA(*(xz + 5));
    vbf = (uint64_t)vbef;
}

int64_t swap_buffers(uint32_t *buffer)
{
    memcpy((uint8_t *)vbf, buffer, SCREENW * SCREENH * 4);
    return 0;
}