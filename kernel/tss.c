#include "include/tss.h"
#include "include/string.h"

extern uint64_t gdt[64];
struct tss tss;
void tss_init()
{
    memset(&tss, 0, sizeof(tss));
    tss.rsp0 = (uint64_t)current->krsp0;
    // tss.rsp0 = current->kstack;
    struct tss_desc *desc = (struct tss_desc *)&gdt[GDT_TSS_ENTRY];
    memset(desc, 0, sizeof(struct tss_desc));
    desc->limit0 = sizeof(tss) & 0xffff;
    desc->base0 = (uint64_t)&tss & 0xffff;
    desc->base1 = ((uint64_t)&tss >> 16) & 0xff;
    desc->type = 0x9;
    desc->p = 1;
    desc->limit1 = (sizeof(tss) >> 16) & 0xf;
    desc->base2 = ((uint64_t)&tss >> 24) & 0xff;
    desc->base3 = ((uint64_t)&tss >> 32) & 0xffffffff;

    __asm__("ltr %w0" ::"r"(GDT_TSS_ENTRY << 3)); // Ti和特权级都是0
}