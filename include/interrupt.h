#pragma once
#include "display.h"
#include "../mm/mm.h"
#include "sched.h"
// 处理器定义
#define T_DIVIDE 0 // 除法错误
#define T_DEBUG 1  // 调试异常
#define T_NMI 2    // 不可屏蔽中断
#define T_BRKPT 3  // 断点
#define T_OFLOW 4  // 溢出
#define T_BOUND 5  // 边界检查
#define T_ILLOP 6  // 非法操作码
#define T_DEVICE 7 // 设备不可用
#define T_DBLFLT 8 // 双重错误
#define T_TSS 10   // 无效的任务切换段
#define T_SEGNP 11 // 段不存在
#define T_STACK 12 // 栈异常
#define T_GPFLT 13 // 通用保护错误
#define T_PGFLT 14 // 页错误
// #define T_RES        15      // 保留
#define T_FPERR 16   // 浮点错误
#define T_ALIGN 17   // 对齐检查
#define T_MCHK 18    // 机器检查
#define T_SIMDERR 19 // SIMD浮点错误

// 自定义向量号
#define T_SYSCALL 128 // 系统调用
#define T_DEFAULT 500 // 默认中断

// x86定义的中断号
#define T_IRQ0 32       // IRQ0中断
#define IRQ_TIMER 0     // 定时器中断
#define IRQ_KBD 1       // 键盘中断
#define IRQ_IDE 14      // IDE中断
#define IRQ_ERROR 19    // 错误中断
#define IRQ_SPURIOUS 31 // 虚假中断

#define GATE_INTERRUPT 0xe      // 中断门
#define GATE_EXCEPTION 0xf      // 异常门
#define COUNTER (1193181 / 100) // 计数器

struct trapframe
{
    // 中断帧的其余部分
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;

    uint64_t trapno;

    // 以下部分由x86硬件定义
    uint64_t err;
    uint64_t rip;
    uint16_t cs;
    uint16_t padding5[3];
    uint64_t eflags;

    // 以下部分仅在跨越特权级别时使用，例如从用户态切换到内核态
    uint64_t rsp;
    uint16_t ss;
    uint16_t padding6[3];
} __attribute__((packed));

struct gate_desc // 中断描述符
{
    uint16_t offset_low;                                             // 偏移量低位
    uint16_t segment;                                                // 段选择子
    uint16_t ist : 3, zero : 5, type : 4, zero2 : 1, dpl : 2, p : 1; // ist:中断索引向量 type:中断类型，dpl:特权级别，p:段存在位
    uint16_t offset_middle;                                          // 偏移量中位
    uint32_t offset_high;                                            // 偏移量高位
    uint32_t reserved;                                               // 保留
} __attribute__((packed));

void init_8254();
void interrupt_init();
void trap(struct trapframe *tf);
void timer_handler();
void pf_handler();
void kbd_handler();
void ide_handler();
void syscall_handler();
void default_handler();
void ox76_handler();
void illop_handler();
int64_t sys_get_ticks();
extern struct gate_desc idt[256]; // 中断描述符表
extern uint64_t ticks;            // 时钟中断次数
