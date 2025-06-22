#include "../include/types.h"
#include "../include/string.h"
#include "../include/asm.h"
#include "include/interrupt.h"
#include "include/sched.h"
#include "../drivers/atkbd.h"

struct gate_desc idt[256]; // 中断描述符表
uint64_t ticks = 0;
static void set_intr_gate(uint8_t index, uint64_t addr, int8_t type)
{
    struct gate_desc *desc = &idt[index];
    memset(desc, 0, sizeof(struct gate_desc));
    desc->segment = 0x08;
    desc->offset_low = (uint16_t)addr;
    desc->offset_middle = (uint16_t)(addr >> 16);
    desc->offset_high = (uint32_t)(addr >> 32);
    desc->type = type;
    desc->dpl = 0;
    desc->p = 1;
}
void init_8254() // 设置为100HZ
{
    outb(0x43, 0x36);
    outb(0x40, COUNTER & 0xff);
    outb(0x40, COUNTER >> 8);
    sprint("timer!\n\r");
}
void interrupt_init()
{
    for (int32_t i = 0; i < 256; i++)
        set_intr_gate(i, (uint64_t)&default_handler, GATE_INTERRUPT); // 统一设置到默认处理
    set_intr_gate(T_PGFLT, (uint64_t)&pf_handler, GATE_EXCEPTION);    // 分别设置有处理函数的
    set_intr_gate(T_IRQ0 + IRQ_TIMER, (uint64_t)&timer_handler, GATE_INTERRUPT);
    set_intr_gate(T_IRQ0 + IRQ_KBD, (uint64_t)&kbd_handler, GATE_INTERRUPT);
    set_intr_gate(T_IRQ0 + IRQ_IDE, (uint64_t)&ide_handler, GATE_INTERRUPT);
    set_intr_gate(T_SYSCALL, (uint64_t)&syscall_handler, GATE_INTERRUPT);
    set_intr_gate(0x76, (uint64_t)&ox76_handler, GATE_INTERRUPT);
    set_intr_gate(0x6, (uint64_t)&illop_handler, GATE_INTERRUPT);
}
void do_timer()
{
    ticks++; // 时钟中断次数 后面得加锁或者信号量
    dprint(ticks);
    printn();

    check_timer(); // 检查有没有需要唤醒的

    schedule(); // 进程时间片调度
    sprint("timer ok!\n\r");
    // int64_t rsp;
    // __asm__("movq %%rsp,%0" : "=r"(rsp));
    // sprint("rsp==");
    // hprint(rsp);
    // hprint(*(int64_t *)rsp);
    // if (ticks >= 13 && current->pid == 1)
    // {
    //     sprint("wtf?!\n");
    //     hlt();
    // }
}
int64_t sys_get_ticks()
{
    sprint("get!!!");
    hprint(ticks);
    return ticks;
}
void do_kbd()
{
    sprint("keboard!\n\r");
    kbd_work();
    return;
}
void do_ide()
{
    return;
}
void do_default() // 不处理的中断和异常
{
    sprint("default!\n\r");
    hlt();
    return;
}
void do_illop()
{
    // sprint("illegal opcode!\n\r");
    // hlt();
    return;
}
void do_pf()
{
    sprint("page fault!\n\r");
    sprint("now: id == ");
    hprint(current->pid);
    uint64_t cr2_value, plm4;
    __asm__ volatile("movq %%cr2, %%rax"
                     : "=a"(cr2_value)
                     :
                     : "memory");
    __asm__ volatile("movq %%cr3, %%rax"
                     : "=a"(plm4)
                     :
                     : "memory");
    sprint("\n\r wrong val ==");
    hprint(cr2_value);
    hlt();
    uint64_t pa = alloc_page();

    sprint("\n\r new pa ==");
    hprint(pa);
    map_range((uint64_t)plm4, cr2_value, pa, 0x4, 1);
    // hlt();
}
void trap(struct trapframe *tf)
{
    // sprint("\n\rtf==");
    // hprint((uint64_t)(tf));
    // hprint((uint64_t)tf->trapno);
    // hprint((uint64_t)tf->rip);
    // hlt();
    if (tf->trapno == T_SYSCALL)
    {
        sprint("system_call\n\r");
        return;
    }
    if (tf->trapno == 0x20 || tf->trapno == 0xe)
    {
        hprint(tf->trapno);
        sprint("\n\rtf==");
        hprint((uint64_t)(tf));
        // hprint((uint64_t)tf->trapno);
        sprint("RIP:");
        hprint((uint64_t)tf->rip);
        sprint("RSP:");
        hprint((uint64_t)tf->rsp);
    }
    switch (tf->trapno)
    {
    case T_IRQ0 + IRQ_TIMER:
        do_timer();
        break;
    case T_IRQ0 + IRQ_KBD:
        do_kbd();
        break;
    case T_IRQ0 + IRQ_IDE:
        do_ide();
        break;
    case T_PGFLT:
        do_pf();
        break;
    case 0x76:
        break;
    case 0x6:
        do_illop();
        break;
    default:
        hprint((uint64_t)tf->trapno);
        hprint((uint64_t)tf->rsp);
        sprint("\n\rtf==");
        hprint((uint64_t)(tf));
        hprint((uint64_t)tf->trapno);
        hprint((uint64_t)tf->rip);
        do_default();
    }
    // if (current->pid == 1)
    // {
    //     hlt();
    // }
    outb(0x20, 0x20); // 发送EOI信号告知中断处理完成
}