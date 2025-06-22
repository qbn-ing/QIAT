#include "include/sched.h"
#include "include/tss.h"
#include "mm/mm.h"
#include "include/string.h"
#include "include/asm.h"
#include "include/interrupt.h"
#include "include/chara.h"

struct pcb *runnable_head = NULL, *runnable_tail = NULL, *sleeping_head, *sleeping_tail;
struct pcb *current, *initpcb = NULL;
uint32_t pcbid[1024];
uint64_t userva = 0x100000, userpa = 0xc800000; // 0号进程的虚拟地址和真实地址
struct timerpcb *timequeue;
void joinTo(struct pcb *p, struct pcb *dst)
{
    if (dst == NULL)
    {
        dst = p;
        return;
    }
    if (dst->next != NULL)
        dst->next->prev = p;
    p->next = dst->next;
    p->prev = dst;
    dst->next = p;
}
void delTo(struct pcb *p, struct pcb *dst)
{
    if (p == dst)
    {
        if (p->next != NULL)
            p->next->prev = NULL;
        dst = p->next;
    }
    else
    {
        p->prev->next = p->next;
        if (p->next != NULL)
            p->next->prev = p->prev;
    }
}
void joinR(struct pcb *p) // 加入到就绪队列
{
    sprint("join:");
    hprint(p->pid);
    if (runnable_head != NULL)
    {
        runnable_tail->next = p;
        p->prev = runnable_tail;
    }
    else
        runnable_head = p;
    runnable_tail = p;
    p->state = RUNNABLE;
}
void delR(struct pcb *p) // 从就绪队列删除
{
    sprint("del:");
    hprint(p->pid);
    if (p == runnable_head)
    {
        if (p == runnable_tail)
        {
            runnable_head = NULL;
            runnable_tail = NULL;
        }
        else
        {
            runnable_head = p->next;
        }
    }
    else
    {
        if (p == runnable_tail)
        {
            runnable_tail = p->prev;
        }
        p->prev->next = p->next;
    }
    sprint("delover!");
}
inline void wait(struct semaphore *s)
{
    __asm__ volatile("cli");
    if (s->counter > 0) // 还有资源
    {
        s->counter--;
        __asm__ volatile("sti");
    }
    else // 没有资源 阻塞
    {
        delR(current);
        current->state = SLEEPING;
        if (s->queue != NULL)
        {
            s->queue->prev = current;
            current->next = s->queue;
        }
        s->queue = current;
        sprint("wating :");
        hprint(current->pid);
        __asm__ volatile("sti");
        schedule();
    }
}
inline void signal(struct semaphore *s)
{
    __asm__ volatile("cli");
    s->counter++;
    if (s->queue != NULL) // 有进程在等待
    {
        struct pcb *p = s->queue;
        s->queue = p->next;
        p->next = NULL;
        sprint("wake up : ");
        hprint(p->pid);
        joinR(p); // 加入到就绪队列
    }
    __asm__ volatile("sti");
}

static void fake_task_stack(uint64_t kstack, uint64_t rip)
{
    uint16_t ss = SEG_UDATA;
    uint64_t rsp = 0xfffeff0;
    uint16_t cs = SEG_UCODE;
    uint64_t rsp_tmp;
    sprint("fake\n\r");
    __asm__("mov %%rsp, %5\n\t"
            "mov %4, %%rsp\n\t"
            "pushq %0\n\t"
            "pushq %1\n\t"
            "pushf\n\t"
            "pushq %2\n\t"
            "pushq %3\n\t"
            "mov %5, %%rsp\n\t"
            :
            : "m"(ss), "m"(rsp), "m"(cs), "m"(rip), "m"(kstack), "m"(rsp_tmp));
}
// 申请进程控制块，入口VA,真实地址，程序空间大小
struct pcb *allocpcb(uint64_t rip, uint64_t rip_pa, uint32_t size) // 申请一个新的进程控制块 申请内核栈和页表
{
    struct pcb *p = malloc(sizeof(struct pcb));
    if (p == NULL)
    {
        sprint("p== NULL\n\r");
        hlt();
        return NULL;
    }
    p->time = 5; // 50ms
    for (volatile int32_t i = 0; i < MAX_TASK; i++)
    {
        if (pcbid[i] == 0)
        {
            pcbid[i] = 1;
            p->pid = i;
            break;
        }
    }
    hprint(p->pid);
    // p->rip = (int64_t)&ret_from_kernel;
    p->firstrun = 1;
    p->kstack = (uint64_t)(VA(alloc_page()) + PAGE_SIZE); // 申请内核栈并指向栈底
    // 伪造中断现场 调用中断处理的中断返回
    fake_task_stack((uint64_t)p->kstack, rip);
    p->krsp0 = p->kstack - 5 * 8;
    p->plm4 = alloc_page();
    sprint("plm4:");
    hprint(p->plm4);
    memset(VA(p->plm4), 0, PAGE_SIZE);
    if (p->pid == 0) // 如果是0号进程，没法复制父进程的页表
    {
        memcpy(VA(p->plm4 + 8 * 256), VA(TASK0_PML4 + 8 * 256), 8 * 256); // 复制内核页表的高256项(0XFFFF800000000000低48位的四级页表是256)
        // 低256项作为用户空间
        // p->parent = NULL;
    }
    else
    {
        // p->parent = current;
        // current->child[current->childnum++] = p;
        memcpy(VA(p->plm4 + 8 * 256), VA(TASK0_PML4 + 8 * 256), 8 * 256); // 复制内核页表的高256项(0XFFFF800000000000低48位的四级页表是256)

        // memcpy(VA(p->plm4), VA(current->plm4), PAGE_SIZE); // 复制父进程的页表,后面进程再自己修改
    }
    if (size != 0)
    {
        map_range(p->plm4, rip, rip_pa, 0x4, (size + PAGE_SIZE) / PAGE_SIZE); // 映射程序所在的位置 0x4是用户模式访问
        uint64_t ursp = alloc_page();
        sprint("pid==");
        hprint(p->pid);
        sprint("usrp==");
        hprint(ursp);
        map_range(p->plm4, 0xffffff0 - 0x1000, ursp, 0x4, 0x1000 / PAGE_SIZE); // 映射用户栈
    }
    // 字库有1536*16=24756个字节,char_bits
    map_range(p->plm4, 0xffffff000, FONTS_ADDR, 0x4, 24756 / PAGE_SIZE);
    sprint("pcb pre over!\n\r");
    return p;
}

// int32_t fork()
// {
//     struct pcb *p = allocpcb(0, 0, 0);
//     if (p == NULL)
//         return -1;
//     // 缺cwd复制
//     // strcpy((int8_t *)p->name, (const int8_t *)current->name, sizeof(current->name));
//     p->state = RUNNABLE; // 还需加锁
//     if (runnable_tail != NULL)
//     {
//         runnable_tail->next = p;
//         p->prev = runnable_tail;
//     }
//     else
//         runnable_head = p;
//     runnable_tail = p;
//     return p->pid;
// }
// void exit()
// {
//     struct pcb *p = current;
//     if (p == initpcb)
//     {
//         sprint("init cann't exit!\n\r");
//         return;
//     }
//     // 关闭打开的文件
//     // 通知父进程
//     // do_wake_up(p->parent);
//     // 把子进程的资源交给init进程
//     for (struct pcb *q = runnable_head; q != NULL; q = q->next)
//     {
//         if (q->parent == p)
//             q->parent = initpcb;
//     }
//     for (struct pcb *q = sleeping_head; q != NULL; q = q->next)
//     {
//         if (q->parent == p)
//             q->parent = initpcb;
//     }
//     // 释放资源
//     // 这里应该是把进程的前256项页表的具体项也释放掉
//     free_page(p->plm4);
//     free_page(p->kstack);
//     free(p);
// }
void switchuvm(struct pcb *p) // 切换页表
{
    sprint("pp");
    hprint(p->pid);
    current = p;
    tss.rsp0 = p->kstack;
    p->state = RUNNING;
    if (p->firstrun == 1)
    {
        p->firstrun = 0;
        hprint(p->krsp0);
        if (p->pid != 0)
            outb(0x20, 0x20);
        __asm__("mov %0, %%cr3\n\t" ::"r"(p->plm4));
        __asm__ volatile("movq %0, %%rsp\n\t"
                         //  "hlt\n\t"
                         "jmp ret_from_kernel\n\t"
                         :
                         : "m"(p->krsp0));
    }
    else
    {
        // sprint("chage cr3!\n\r");
        __asm__("mov %0, %%cr3\n\t" ::"r"(p->plm4));
    }
}
void userinit() // 进程创建
{
    sprint("start!\n\r");
    for (volatile int32_t i = 0; i < MAX_TASK; i++)
        pcbid[i] = 0;
    struct pcb *p = allocpcb(userva, userpa, 1024 * 20);
    hprint((uint64_t)p);
    initpcb = p;
    strcpy((int8_t *)p->name, "systemd", sizeof("systemd"));
    current = p;
    p->state = RUNNABLE;
    tss_init();
    struct pcb *p2 = allocpcb(0x100000, 0xc820000, 1024 * 25);
    struct pcb *p1 = allocpcb(0x100000, 0xc810000, 1024 * 20);
    cli();
    joinR(p2);
    joinR(p1);
    switchuvm(p);
}
void schedule()
{
    sprint("schedule!\n\r");
    if (initpcb == NULL) // 无法调度
    {
        sprint("\n\rinitnull\n\r");
        return;
    }
    if (current->time > 1 && current != initpcb && current->state == RUNNING) // 时间片减少
    {
        current->time--;
        hprint(current->pid);
        sprint("timepace:");
        hprint(current->time);
        return;
    }
    if (current->state == RUNNING)
    {
        current->state = RUNNABLE;
        if (current != initpcb)
            joinR(current); // 加入到就绪队列
        current->time = 5;
    }
    struct pcb *p = runnable_head;
    while (p != NULL)
    {
        sprint("new:");
        hprint(p->pid);
        if (p->state == RUNNABLE)
        {
            delR(p); // 从就绪队列移除
            break;
        }

        p = p->next;
    }
    if (p == runnable_head && initpcb != NULL)
    {
        sprint("now init!!!\n\r");
        p = initpcb;
    }
    sprint("p->ksatc:");
    hprint(p->kstack);
    sprint("p->krsp0:");
    hprint(p->krsp0);
    __asm__("movq %%rsp,%0" ::"m"(current->krsp0)); // 保存内核栈栈顶
    // sprint("krsp0===");
    // hprint(current->krsp0);
    switchuvm(p);
    __asm__("movq %0,%%rsp\n\t"
            // "hlt\n\t"
            :
            : "m"(p->krsp0));
    sprint("over!\n\r");
}
void check_timer()
{
    if (timequeue == NULL)
        return;
    struct timerpcb *head = timequeue;

    while (head != NULL)
    {
        head->lefttime -= 10;
        sprint("pid==");
        hprint(head->pcb->pid);
        sprint("lefttime==");
        dprint(head->lefttime);
        printn();
        if (head->lefttime <= 0)
        {
            joinR(head->pcb);
            if (head->prev != NULL)
                head->prev->next = head->next;
            else // 如果是第一个就把头指向下一个
                timequeue = head->next;
            if (head->next != NULL)
                head->next->prev = head->prev;
        }
        head = head->next;
    }
    sprint("check over!\n\r");
}
int64_t sys_sleep(int32_t time)
{
    struct timerpcb *timer_t = malloc(sizeof(struct timerpcb));
    timer_t->pcb = current;
    timer_t->lefttime = time;
    if (timequeue != NULL)
    {
        timequeue->prev = timer_t;
        timer_t->next = timequeue;
    }
    timequeue = timer_t;
    current->state = SLEEPING;
    schedule();
    return 0;
}
int64_t sys_getpid()
{
    return current->pid;
}