#include "../include/asm.h"
#include "../include/string.h"
#include "../mm/mm.h"
#include "../include/display.h"
#include "../include/interrupt.h"
#include "../include/readfile.h"
#include "../include/sched.h"
#include "../include/tss.h"
#include "../include/syscall.h"
#include "../drivers/atkbd.h"
#include "../drivers/vesa.h"
#include "../include/chara.h"
#include "../include/math.h"

void main64()
{
    mm_init();
    vga_init();
    readfile((uint8_t *)(VA(0x200000)), 0x6000, 100);     // 读取字库
    readfile((uint8_t *)(VA(0xc800000)), 1024 * 20, 105); // 作为user0
    readfile((uint8_t *)(VA(0xc810000)), 1024 * 20, 136); // 作为2
    readfile((uint8_t *)(VA(0xc820000)), 1024 * 25, 160); // 作为1
    // hlt();
    kbd_init();
    interrupt_init();
    syscall_init();

    uint32_t *gtf = (uint32_t *)sys_malloc_big0(SCREENH * SCREENW * DEPTH, TASK0_PML4);
    set_fb((uint64_t)gtf);
    clear_screen();
    setChar_bits((uint8_t *)VA(0x200000));

    draw_rect(100, 100, 800, 800, 255, 0, 255, 128);
    draw_rect(10, 10, 700, 700, 200, 100, 255, 128);
    prints1("Ciallo!", 0, 0, 0);
    hprint((uint64_t)&fb_addr);
    sprint("ciallo\n\r");

    for (int i = 500; i < 800; i++)
        draw_line(i, 500, i, 800, 255, 255, 255, 255);
    swap_buffers(gtf);

    sprint("\n\rover\n\r");
    init_8254();
    sti();

    userinit();

    hlt();
}