#pragma once

#include "types.h"
#include "tss.h"
#include "mmu.h"

#define MAX_TASK 1024

// struct context // 进程切换现场
// {
//     uint64_t rdi;
//     uint64_t rsi;
//     uint64_t rbp;
//     uint64_t rbx;
//     uint64_t rip;
// };

enum procstate
{
    RUNNABLE, // 就绪态 0
    RUNNING,  // 运行 1
    SLEEPING, // 阻塞 2
    // ZOMBIE    // 僵尸 3
};

struct pcb
{
    uint64_t pid; // 进程id
    uint8_t time; // 剩余时间片
    // struct context *context; // 进程上下文
    uint64_t plm4; // 进程页表地址
    // uint64_t sz;          // 进程内存大小
    uint64_t kstack;      // 进程内核栈
    uint64_t krsp0;       // 内核栈栈顶
    enum procstate state; // 进程状态
    uint64_t rsp;         // 进程栈指针
    // struct pcb *parent;    // 进程父进程
    // struct pcb *child;     // 进程子进程
    // struct pcb *childnext; // 进程子进程链表
    // uint32_t childnum;     // 子进程数量
    uint8_t name[16]; // 进程名
    // uint64_t rip;     // 进程的指针
    int32_t firstrun; // 进程是否是第一次运行
    struct pcb *next;
    struct pcb *prev;
    // struct inde *cwd;//工作目录
};

struct timerpcb
{
    struct pcb *pcb;
    int32_t lefttime;
    struct timerpcb *prev;
    struct timerpcb *next;
};

struct semaphore
{
    int32_t counter;
    struct pcb *queue;
};
void wait(struct semaphore *s);
void signal(struct semaphore *s);
extern struct pcb *current;
extern struct timerpcb *timequeue;

void userinit();                 // 0号进程的创建
void ret_from_kernel();          // 内核态返回用户态
void schedule();                 // 进程调度
void check_timer();              // 检查有没有延时
int64_t sys_sleep(int32_t time); // 进程休眠time毫秒
int64_t sys_getpid();            // 获得程序id
// void do_wake_up(struct pcb *p);  // 唤醒进程
// 上面俩还没想好怎么实现
