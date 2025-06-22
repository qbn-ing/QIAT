```bash
#     ___                 _       _____   
#    / " \     ___    U  /"\  u  |_ " _|  
#   | |"| |   |_"_|    \/ _ \/     | |    
#  /| |_| |\   | |     / ___ \    /| |\   
#  U \__\_\u U/| |\u  /_/   \_\  u |_|U   
#     \\//.-,_|___|_,-.\\    >>  _// \\_  
#    (_(__)\_)-' '-(_/(__)  (__)(__) (__) 
```
# QIAT:QIAT Is A Toy

------------------------------------------------------

## 目录

[TOC]

-------------------------------------------------------------

## 0.开发环境及文件组织

### a.开发环境

· Kubuntu 23.10

· gcc 12.2

· VS Code

· qemu-system-x86_64

### b.文件

```bash
QIAT
├── apps # 用户程序
│   ├── app1.c
│   ├── app3.c
│   └── app4.c
├── boot # 引导程序
│   ├── boot32.c # 用于加载32位内核
│   ├── boot32.S # 完成初步的内核映射、恒等映射和线性映射，开启分页模式和长模式
│   ├── bootblock.S # 引导扇区
│   ├── bootloader.c # 用于加载bootloader
│   ├── bootloader.S # 获取硬件信息，初始化显卡，开启保护模式
│   ├── check.pl # 保证引导扇区不会过大
│   ├── fdisk.txt # 创建虚拟磁盘需要的参数
│   └── head64.c # 用于加载64位内核
├── drafts # 草稿及未完成的部分
│   ├── fs
│   │   ├── fs.c
│   │   ├── fs.h
│   │   └── fsbuild.c
│   └── char_paint.txt
├── drivers # 硬件相关
│   ├── atkbd.c # 键盘相关
│   ├── atkbd.h
│   ├── speaker.c # 扬声器相关
│   ├── speaker.h
│   ├── vesa.c # 显卡相关
│   └── vesa.h
├── fonts # 共享点阵字库
│   └── char2.c
├── include
│   ├── asm.h # 一些内联汇编的函数封装
│   ├── chara.h 
│   ├── display.h
│   ├── interrupt.h
│   ├── math.h # 一些数学函数
│   ├── mmu.h # 内存的相关定义
│   ├── queue.h # 简单的循环队列
│   ├── readfile.h # 从磁盘指定扇区读取文件
│   ├── sched.h # 进程相关 
│   ├── string.h # 常用字符串内存相关函数
│   ├── syscall.h # 系统调用
│   ├── tss.h # 任务状态段相关
│   └── types.h # 一些基本类型的定义
├──  kernel 
│   ├──  handler.S # 中断处理的汇编文件
│   ├──  head64.S # 64位内核头
│   ├──  interrupt.c # 中断处理
│   ├──  main.c # 64位内核C语言部分
│   ├──  sched.c # 进程相关
│   ├──  syscall.c # 系统调用
│   └──  tss.c # 任务状态段
├──  lib
│   ├──  display.c # 显示相关
│   ├──  math.c # 简单的随机数生成
│   ├──  queue.c # 简单的循环队列
│   ├──  std.h # 系统调用函数的头文件
│   ├──  string.c
│   └──  syscall.S # 系统调用的汇编文件
├──  mm # 内存相关
│   ├──  malloc.c # 内存分配
│   ├──  mm.c # 内存映射
│   └──  mm.h
├──  init.gdb # gdb调试脚本
├──  Makefile # 可以通过make run来启动程序
└──  os.img   # 内核系统映像

```

### c.各模块在内存以及磁盘的位置

| 作用         | 内存物理地址 | 磁盘起始位置(扇区) |
| ------------ | ------------ | :----------------: |
| bootblock    | `0x7c00`     |        `0`         |
| bootloader   | `0x7e00`     |        `1`         |
| 内存信息     | `0x9000`     |         -          |
| 显卡信息     | `0x9200`     |         -          |
| boot32       | `0xa000`     |        `3`         |
| 内核四级页表 | `0x10000`    |         -          |
| head64       | `0x100000`   |        `5`         |
| 共享字库     | `0x200000`   |       `100`        |
| app3         | `0xc800000`  |       `105`        |
| app4         | `0xc810000`  |       `136`        |
| app1         | `0xc820000`  |       `160`        |

## 1.内核实现的功能

### a.内存管理

内核最终运行在64位模式下，启用了分页机制，使用四级页表，未实现虚拟内存功能。  
共分配1个四级页表，位于0x10000处  
共分配3个三级页表，位于0x11000处，用于内核映射  
                			位于0x33000处，用于恒等映射  
                			位于0x35000处，用于线性映射  
共分配3个二级页表，位于0x12000处，用于内核映射  
                			位于0x34000处，用于恒等映射  
                			位于0x36000处，用于线性映射  
共分配2个一级页表，位于0x13000处，用于内核映射和恒等映射  
                			位于0x370000处，用于线性映射  

实现了内存分配函数

相关文件：mm/mm\.c  mm/malloc\.c

### b.中断和系统调用

实现了时钟中断、键盘中断、页错误的中断服务程序。

相关文件：kernel/handle.S kernel/interrupt\.c 

实现了进程休眠、申请内存、交换帧缓冲、获得当前滴答数、获得键盘输入的系统调用

相关文件：kernel/syscall.c lib/syscall.S lib/std\.h

### c.进程管理

实现了进程的创建、阻塞、唤醒、时间片轮转调度，实现了简单的信号量

相关文件：kernel/sched.c kernel/tss\.c

### d.设备管理

将VGA显卡设置为1440\*900\*32模式

实现了扬声器的beep函数

实现了简易的键盘输入

相关文件：drivers/*

### e.库函数

 实现了从磁盘读文件、快速幂、随机数、申请帧缓冲、绘制像素点、绘制线、绘制矩形、绘制心形、绘制8*16的文本、简单的循环队列、内存赋值、内存复制，以及一些字符串相关的函数

相关文件：lib/*.c include/readfile.h

## 2.内核模块

### a. boot :引导和初始化模块

位于boot/文件夹下，bootblock\.S位于0号扇区，此时位于16位模式下，通过bootloader\.c把bootloader\.S加载到`0x7e00`处。check.pl保证bootblock不超过1个扇区。

进入bootloader后，逐步完成开启A20，读取内存信息并保存到`0x9000`处，设置显示模式并把显示信息保存到`0x9200`处，初始化8295A，然后设置好gdt表，开启保护模式，将32位的boot32.S读入`0xa000`处，长跳转进入32位保护模式

在32位保护模式里，完成内核映射、恒定映射和线性映射的第一部分，然后开启分页模式，读取64位的kernel/head64.S至`0x100000`处，跳转进入64位

### b. kernel：内核相关模块

进入head64.S后，分别设置gdt和idt，设置好内核栈地址，然后通过`push` `ret`的方式，跳转入内核映射的高地址空间里，进入main.c

进入main.c后，首先调用内存初始化函数，完成所有4G空间的线性映射，然后获取帧缓冲地址，然后将三个用户程序加载到内存的指定位置。接下来初始化中断，初始化系统调用，然后往屏幕绘制文字和矩形来测试是否正确进行。接着是设置时钟中断和启用中断。启用中断后，进入用户程序初始化阶段。

用户程序初始化在sched.c文件里实现，申请了新的进程控制块，为新进程申请了内核栈，并伪造中断现场，为新进程新建了页表，把内核高地址复制到新进程的页表里，为进程申请进程栈，然后把共享字库映射到默认位置。进程初始化完成后，调用switchuvm切换到用户进程的页表，然后根据进程是否为第一次运行选择不同的切换方式。当程序是第一次运行时，跳转到handler.S里的ret_from_kernel标签，通过iretq来把rsp和rip弹出来。如果不是第一次运行的，那么需要先保留好内核栈栈顶，然后切换页表，再切换回保存的内核栈栈顶即可。

### c. apps: 用户程序

 在Makefile文件里，将app3.bin放到第106扇区到136扇区

​				     将app4.bin放到第137扇区到160扇区

​				     将app1.bin放到第160扇区及往后

在main.c里，分别又从磁盘中读出三个程序，分别存入`0xc800000`作为0号进程，存入`0xc810000`和`0xc820000`作为后续两个进程

app3里仅仅进行死循环的往串口输出

app4里通过蜂鸣器循环播放《两只老虎》

app1里实现了黑客帝国数字雨的开局，然后后续通过键盘进入生命游戏，可以通过键盘生成随机的生命游戏开局。

## 3. 内核启动流程图

![qiat](.\picture\qiat.png)

## 4.部分代码

### 基础变量类型的规定

```c
typedef char int8_t;
typedef short int int16_t;
typedef int int32_t;
typedef long int int64_t;

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long int uint64_t;

#define NULL ((void *)0)
```

### a.进程相关

```c
enum procstate
{
    RUNNABLE, // 就绪态 0
    RUNNING,  // 运行 1
    SLEEPING, // 阻塞 2
};

struct pcb//进程控制块
{
    uint64_t pid; // 进程id
    uint8_t time; // 剩余时间片
    uint64_t plm4; // 进程页表地址
    uint64_t kstack;      // 进程内核栈
    uint64_t krsp0;       // 内核栈栈顶
    enum procstate state; // 进程状态
    uint64_t rsp;         // 进程栈指针
    uint8_t name[16]; // 进程名
    int32_t firstrun; // 进程是否是第一次运行
    struct pcb *next;
    struct pcb *prev;
};

struct timerpcb //sleep用到的阻塞队列
{
    struct pcb *pcb;
    int32_t lefttime; //剩余时间
    struct timerpcb *prev;
    struct timerpcb *next;
};

struct semaphore //信号量
{
    int32_t counter;
    struct pcb *queue;
};
struct pcb *runnable_head = NULL, *runnable_tail = NULL;
//就绪队列
struct pcb *current, *initpcb = NULL;
//当前进程 0号进程
uint32_t pcbid[1024];//pid是否被选用
struct timerpcb *timequeue;//sleep阻塞队列
```

进程创建

```c
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
    for (volatile int32_t i = 0; i < MAX_TASK; i++)//寻找可用的pid
    {
        if (pcbid[i] == 0)
        {
            pcbid[i] = 1;
            p->pid = i;
            break;
        }
    }
    p->firstrun = 1;//刚创建的程序第一次运行
    p->kstack = (uint64_t)(VA(alloc_page()) + PAGE_SIZE); // 申请内核栈并指向栈底
    // 伪造中断现场 调用中断处理的中断返回
    fake_task_stack((uint64_t)p->kstack, rip);
    p->krsp0 = p->kstack - 5 * 8;
    p->plm4 = alloc_page();//申请一个新的四级页表
    memset(VA(p->plm4), 0, PAGE_SIZE);
    memcpy(VA(p->plm4 + 8 * 256), VA(TASK0_PML4 + 8 * 256), 8 * 256); // 复制内核页表的高256项(0XFFFF800000000000低48位的四级页表是256)
    if (size != 0)
    {
        map_range(p->plm4, rip, rip_pa, 0x4, (size + PAGE_SIZE) / PAGE_SIZE); // 映射程序所在的位置 0x4是用户模式访问
        uint64_t ursp = alloc_page();
        map_range(p->plm4, 0xffffff0 - 0x1000, ursp, 0x4, 0x1000 / PAGE_SIZE); // 映射用户栈
    }
    // 字库有1536*16=24756个字节,char_bits
    map_range(p->plm4, 0xffffff000, FONTS_ADDR, 0x4, 24756 / PAGE_SIZE);
    return p;
}
```

由于此时进程比较简单，没有shell也就不会在系统进行时新建进程。而如果实现了shell那么新进程的页表应该先复制父进程的页表，此处并没有实现进程树，故每个进程都会先把内核映射部分先复制到进程页表中。此处为方便管理，规定`0xffffff000`为用户进程内核栈栈底，`0xffffff000`为共享点阵字库的地址。

### b.图像显示

```c
void vga_init()
{
    uint64_t *xz = VA(0x9200);
    uint32_t *vbef = (uint32_t *)VA(*(xz + 5));//此处为帧缓冲地址
    vbf = (uint64_t)vbef;
}
```

获得帧缓冲地址

```c
uint32_t *fb_addr;//绘制目标的帧缓冲地址
uint8_t *chars;//字库地址
```

设计图像显示相关功能时，在内核模式曾经绘制蔡徐坤和一个动漫人物的字符画，但是卡顿感严重。后面采用虚拟帧，即将绘制函数作为库函数，取而代之的是添加了获取一个虚拟帧的函数和交换帧的系统调用。图像绘制性能得到提升。早期设计中是打算将屏幕分片，这样不同程序占用不同的部分，分配自己的虚拟帧的想法也是从此产生，碍于shell并没有实现，所以此处的帧是整个屏幕的帧

```c
int64_t swap_buffers(uint32_t *buffer)
{
    memcpy((uint8_t *)vbf, buffer, SCREENW * SCREENH * 4);
    return 0;
}
```

`vbf`是显卡帧缓冲地址，往该地址写入数据即可在屏幕显示像素

```C
inline uint32_t *get_buffer()
{
    fb_addr = (uint32_t *)umalloc(SCREENH * SCREENW * DEPTH);
    clear_screen();
    setChar_bits((uint8_t *)0xffffff000);
    return fb_addr;
}
```

用户进程如果要往屏幕绘制，则需要申请一个自己的虚拟帧缓冲，该函数顺便设置了用户程序的点阵字库位置，方便后续的文字绘制

````C
void draw_point(int32_t x, int32_t y, int8_t r, int8_t g, int8_t b, int8_t a)
{
    volatile uint32_t *tmpfb = fb_addr + SCREENW * y + x;
    *((int8_t *)tmpfb + 0) = b;
    *((int8_t *)tmpfb + 1) = g;
    *((int8_t *)tmpfb + 2) = r;
    *((int8_t *)tmpfb + 3) = a;
}
````

这是往帧缓冲绘制点的函数，其他所有绘制相关函数均以此为基础

### c.简单的循环队列

```c
typedef struct cqueue
{
    int8_t *buf;     // 传入一个数组作为队列
    uint32_t length; // 必须传入长度，循环队列
    uint32_t head;   // 出队
    uint32_t tail;   // 入队
} cqueue_t;          // 循环队列
//相关函数
inline static uint32_t queue_next(cqueue_t *q, uint32_t index)//循环取下一个
{
    return (index + 1) % q->length;
}
void queue_init(cqueue_t *q, int8_t *buf, uint32_t length)//初始化队列
{
    q->buf = buf;
    q->length = length;
    q->head = 0;
    q->tail = 0;
}

int8_t queue_pop(cqueue_t *q)//出队
{
    if (queue_empty(q))
        return -1;
    int8_t byte = q->buf[q->head];
    q->head = queue_next(q, q->head);
    return byte;
}

void queue_push(cqueue_t *q, int8_t byte)//入队
{
    while (queue_full(q))
    {
        queue_pop(q);
    }
    q->buf[q->tail] = byte;
    q->tail = queue_next(q, q->tail);
}

int8_t queue_empty(cqueue_t *q)//判断队列为空
{
    return q->head == q->tail;
}

int8_t queue_full(cqueue_t *q)//判断队列为满
{
    return queue_next(q, q->tail) == q->head;
}
```

### d.进程调度

```c
void switchuvm(struct pcb *p) // 切换页表
{
    current = p;
    tss.rsp0 = p->kstack;//切换内核栈
    p->state = RUNNING;
    if (p->firstrun == 1)
    {
        p->firstrun = 0;//标记不是第一次运行了
        if (p->pid != 0)//只有0号进程不是通过中断来进行调度的
            outb(0x20, 0x20);//通知中断处理完成
        __asm__("mov %0, %%cr3\n\t" ::"r"(p->plm4));
        __asm__ volatile("movq %0, %%rsp\n\t"
                         "jmp ret_from_kernel\n\t"
                         :
                         : "m"(p->krsp0));
    }
    else
    {
        __asm__("mov %0, %%cr3\n\t" ::"r"(p->plm4));
    }
}
void schedule()
{
    if (initpcb == NULL) // 0号进程无效，无法调度
    {
        sprint("\n\rinitnull\n\r");
        return;
    }
    if (current->time > 1 && current != initpcb && current->state == RUNNING) // 时间片减少
    {
        current->time--;
        return;
    }
    if (current->state == RUNNING)
    {
        current->state = RUNNABLE;//设置为就绪态
        if (current != initpcb)
            joinR(current); // 加入到就绪队列
        current->time = 5;//设置为50ms
    }
    struct pcb *p = runnable_head;
    while (p != NULL)//寻找就绪队列中第一个不为空的进程
    {
        if (p->state == RUNNABLE)
        {
            delR(p); // 从就绪队列移除
            break;
        }
        p = p->next;
    }
    if (p == runnable_head && initpcb != NULL)//没有就绪进程时
    {
        p = initpcb;
    }
    __asm__("movq %%rsp,%0" ::"m"(current->krsp0)); // 保存内核栈栈顶
    switchuvm(p);
    __asm__("movq %0,%%rsp\n\t"//恢复要切换的进程的内核栈栈顶
            :
            : "m"(p->krsp0));
}
```

在进行进程调度时曾经遇到过一些麻烦，因为第一次启动某个用户程序时需要从内核态切入用户态，所以需要进行中断现场的伪造，然后再通过“假”中断返回，切入用户程序。而非第一次启动的时候，进程一定是通过中断程序或sleep造成的进程阻塞来到此处的，此时中断机制已经贴心地把用户态的栈、pc等值保存到了内核栈里，而内核栈不仅保存了用户程序的一些信息，在到达调度程序的过程中，这一路的函数调用也都保存在了内核栈里，而如果不进行保存，那么再次进入该进程的时候其实也是不知道怎么进入内核栈的。而通过恢复保存内核栈，对于之前被调度走的程序来看，它的内核栈又变成进入调度程序的栈了，意味着它可以正常地从调度程序返回，再通过中断机制的返回，回到被调度之前的状态

### e.蜂鸣器播放

```c
void beep(uint16_t freq, int32_t durat)//beep函数的实现
{
    uint16_t counter = 1193182 / freq;
    outb(PIT_CTRL_PORT, 0b10110110);
    outb(PIT_CHAN2_PORT, (uint8_t)counter);//设置播放频率
    outb(PIT_CHAN2_PORT, (uint8_t)(counter >> 8));
    if (!beeping)
    {
        outb(SPEAKER_PORT, in(SPEAKER_PORT) | 0b11);//开始播放
        beeping = 1;
        sleep(durat);
        outb(SPEAKER_PORT, in(SPEAKER_PORT) & 0xfc);//停止播放
        beeping = 0;
    }
}
```

播放两只老虎依赖于参考资料c里提供的不同音符频率

```c
beep(do, pai);//两
beep(re, pai);//只
beep(mi, pai);//老
beep(do, pai);//虎
sleep(ting);
beep(do, pai);//两
beep(re, pai);//只
beep(mi, pai);//老
beep(do, pai);//虎
sleep(ting);
beep(mi, pai);//跑
beep(fa, pai);//得
beep(so, pai * 2);//快
sleep(ting);
beep(mi, pai);//跑
beep(fa, pai);//得
beep(so, pai * 2);//快
sleep(ting);
beep(so, pai2);//一
beep(la, pai3);//只
beep(so, pai2);//没
beep(fa, pai3);//有
beep(mi, pai);//眼
beep(do, pai);//睛
sleep(ting);
beep(so, pai2);//一
beep(la, pai3);//只
beep(so, pai2);//没
beep(fa, pai3);//有
beep(mi, pai);//尾
beep(do, pai);//巴
sleep(ting);
beep(do, pai);//真
beep(so, pai);//奇
beep(do, pai * 2);//怪
sleep(ting);
beep(do, pai);//真
beep(so, pai);//奇
beep(do, pai * 2);//怪
sleep(ting);
```

### f.中断处理

```C
struct trapframe
{
    // 保存的寄存器
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

    uint64_t trapno;//中断号

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

```

仿照xv6系统构建了中断帧，方便在C语言完成的中断处理程序中查看保存的值

```assembly
alltraps:
    #构建中断帧
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    #设置数据段
    movq $0x10, %rax
    movq %rax, %ds
    movq %rax, %es
    #trap通过rsp访问中断帧
    movq %rsp, %rdi//传递中断帧地址

    call trap//转C语言的中断处理函数

trapret:
    #恢复寄存器
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rbp
    popq %rdi
    popq %rsi
    popq %rdx
    popq %rcx
    popq %rbx
    popq %rax
    addq $0x10, %rsp //跳过中断号和错误码  
    iretq
//这里举一个简单的中断处理程序的汇编部分    
timer_handler:
    pushq $0 //错误码
    pushq $32 //中断号
    jmp alltraps
```

构建中断帧以及恢复寄存器，以及每个中断都有类似的中断处理程序

```c
void trap(struct trapframe *tf)
{
    switch (tf->trapno)
    {
    case T_IRQ0 + IRQ_TIMER:
        do_timer(); // 时钟
        break;
    case T_IRQ0 + IRQ_KBD:
        do_kbd(); // 键盘
        break;
    case T_IRQ0 + IRQ_IDE:
        do_ide(); // 硬盘
        break;
    case T_PGFLT:
        do_pf(); // 页错误
        break;
    case 0x76://这里是一个未知的中断信号
        break;
    case 0x6://非法指令中断信号
        do_illop();
        break;
    default:
        do_default();
    }
    outb(0x20, 0x20); // 发送EOI信号告知中断处理完成
}
void do_default() // 不处理的中断和异常
{
    sprint("default!\n\r");
    hlt();
    return;
}
void do_timer()
{
    ticks++; // 时钟中断次数
    check_timer(); // 检查有没有需要唤醒的
    schedule(); // 进程时间片调度
}
void do_pf() // 页错误
{
    sprint("page fault!\n\r");
    sprint("now: id == ");
    hprint(current->pid); // 串口输出当前进程id
    uint64_t cr2_value, plm4;
    __asm__ volatile("movq %%cr2, %%rax"
                     : "=a"(cr2_value)
                     :
                     : "memory");
    __asm__ volatile("movq %%cr3, %%rax"
                     : "=a"(plm4)
                     :
                     : "memory");
    sprint("\n\r wrong val =="); // 页错误的值，一般是访问一个无意义的内存地址
    hprint(cr2_value);
    hlt(); // 没有实现虚拟内存，所以出现页错误必然是代码错误，暂停找错误
    uint64_t pa = alloc_page(); // 申请一个页
    sprint("\n\r new pa ==");
    hprint(pa);
    map_range((uint64_t)plm4, cr2_value, pa, 0x4, 1); // 把这个新页映射给页错误的值
}
```

> 一切皆为状态机

既然操作系统内核也是一个状态机，那么应该不存在不可预知的行为，规定之外的异常和中断大概率就是代码编写问题，于是对于不处理的中断和异常，系统此时会调用`hlt()`函数，也就是进入一个空循环，此时可以用qemu相关命令来进行查错

### g.系统调用

```C
// 系统调用号
#define SYS_sleep 1
#define SYS_display 2
#define SYS_umalloc 3
#define SYS_get_ticks 4
#define SYS_gets 5
#define SYS_getchar 6

typedef int64_t (*fn_ptr)();
extern int64_t sys_sleep(int32_t time);  				//进程休眠
extern int64_t swap_buffers(uint32_t *buffer);			//交换帧缓冲
extern int64_t sys_malloc_big(int32_t size);			//用户申请内存
extern int64_t sys_get_ticks();					      	//获得时钟滴答数
extern int64_t gets_kbd(int8_t *buf, uint32_t count);	//获取键盘缓冲区--目前存在bug
extern int64_t get_kbd();								//获得键盘缓冲区的一个字符
fn_ptr syscall_table[] = {//系统调用表
    [SYS_sleep] sys_sleep,
    [SYS_display] swap_buffers,
    [SYS_umalloc] sys_malloc_big,
    [SYS_get_ticks] sys_get_ticks,
    [SYS_gets] gets_kbd,
    [SYS_getchar] get_kbd,
};
```

系统调用号是完成系统调用的先后顺序得到的，目前gets存在问题，目前也没有实现从缓冲区取若干字符的应用，所以一般使用的get来获取一个字符

```assembly
.code64
#define SYS_sleep 1
#define SYS_display 2
#define SYS_umalloc 3
#define SYS_get_ticks 4
#define SYS_gets 5
#define SYS_getchar 6

#define SYSCALL(name) \
  .globl name; \
  name: \
    movq $SYS_ ## name, %rax; \
    syscall ;\
    ret ;\

SYSCALL(sleep)
SYSCALL(display)
SYSCALL(umalloc)
SYSCALL(get_ticks)
SYSCALL(gets)
SYSCALL(getchar)
```

这里无法使用`int 0x80`,只能使用`syscall`，使用宏来生成各个系统调用的汇编使用代码

```assembly
.macro alltrapsa//不保存rax
    pushq %rdi
    pushq %rsi
    pushq %rdx
    pushq %rcx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
.endm

.macro trapsreta//不恢复rax
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rcx
    popq %rdx
    popq %rsi
    popq %rdi
    //hlt
.endm

syscall_handler:
    cli
    mov %rsp, tss + 20 //保存当前用户栈
    mov tss + 4, %rsp  //切换为内核栈
    pushq tss + 20    //保存用户栈
    alltrapsa // 保存上下文
    call *syscall_table(, %rax, 8) //查找系统调用表
    trapsreta //恢复上下文
    popq %rsp
    sysretq
```

这里的上下文保存和恢复同中断处理存在差异，因为中断号由`rax`寄存器传递

### h.内存映射

```assembly
 cld//设置递增
  // 四级页表
  mov $0x10000, %edi//恒等映射
  mov $0x33003, %eax
  stosl
  mov $0x10888, %edi//线性映射
  mov $0x35003, %eax
  stosl
  mov $0x10ff8, %edi//内核映射
  mov $0x11003, %eax
  stosl
// 三级页表
  mov $0x11ff0, %edi//内核映射
  mov $0x12003, %eax
  stosl
  mov $0x33000, %edi//恒等映射
  mov $0x34003, %eax
  stosl
  mov $0x35000, %edi//线性映射
  mov $0x36003, %eax
  stosl
// 二级页表
  //内核映射
  mov $0x12000, %edi
  mov $0x13000 + 3, %eax
1:
  stosl //将eax的值存入edi指向的地址，然后edi+4
  add $0x1000, %eax
  add $4, %edi
  cmp $(0x12000 + 32 * 8 - 8), %edi //建立32个页表项
  jle 1b
  //恒等映射
  mov $0x34000, %edi
  mov $0x13003, %eax//复用内核映像的一级页表
  stosl
  //线性映射
  mov $0x36000, %edi
  mov $0x13000 + 3, %eax //复用内核映像的一级页表
3:
  stosl
  add $0x1000, %eax
  add $4, %edi
  cmp $(0x36000 + 32 * 8 - 8), %edi
  jle 3b
  
  mov $0x36100, %edi //第33项
  mov $0x37003, %eax
  stosl
// 一级页表
  mov $0x13000, %edi
  mov $0x0 + 3, %eax
2:
  stosl
  add $0x1000, %eax
  add $4,%edi
  cmp $(0x13000+512*8*32-8),%edi //为这32个页表项建立512个页表项 共64MB
  jle 2b
```

32位内核建立了三个映射，线性映射仅仅多完成了一张空余一级页表，进入64位内核后，由`mm_init`完成总内存的获取和全部内存的线性映射

```c
// 完成以页面为粒度的虚拟地址到物理地址的映射
// pml4_pa 根页表的物理地址 from_va 需要映射的起始虚拟地址  to_pa 对应的物理地址 us 访问权限 npage 映射的页面数
void map_range(uint64_t pml4_pa, uint64_t from_va, uint64_t to_pa, int8_t us, int64_t npage)
{
    volatile int64_t n = 0;
    while (n < npage)
    {
        // pml4 40-48位
        uint64_t *page_va = VA(pml4_pa);         // 找到页表基址的虚拟地址
        int16_t index = (from_va >> 39) & 0x1ff; // 仅保留虚拟地址当前的9位
        uint64_t entry = *(page_va + index);     //*(page_va + index); // 基址加索引得到对应的页表项

        if (!entry & 0x1) // 页表项不存在
        {
            *(page_va + index) = alloc_page() | 0x3 | us; // 分配一个页面并设置访问权限
            entry = *(page_va + index);                   // 在分配的页面那里建立后面的页表
        }
        // pml3 31-39位
        page_va = VA(entry & 0xffffffffff000);
        index = (from_va >> 30) & 0x1ff;
        entry = *(page_va + index);
        if (!(entry & 0x1))
        {
            *(page_va + index) = alloc_page() | 0x3 | us;
            entry = *(page_va + index);
        }
        // pml2 22-30位
        page_va = VA(entry & 0xffffffffff000);
        index = (from_va >> 21) & 0x1ff;
        entry = *(page_va + index);
        if (!(entry & 0x1))
        {
            *(page_va + index) = alloc_page() | 0x3 | us;
            entry = *(page_va + index);
        }
        // pml1 13-21位
        page_va = VA(entry & 0xffffffffff000);
        index = (from_va >> 12) & 0x1ff;
        entry = *(page_va + index);
        if (!(entry & 0x1)) // 页表项不存在
        {
            *(page_va + index) = (to_pa + PAGE_SIZE * n) | 0x3 | us; // 当前页表项的物理地址
        }
        n++; // 映射完一个页面
        from_va += PAGE_SIZE;
    }
}
void mm_init()
{
    struct e820map *e820 = (struct e820map *)0x9000;

    for (int32_t i = 0; i < e820->nr_entry; i++) // 得到内存大小
    {
        if (e820->map[i].type == E820_RAM)
        {
            uint64_t tmp = e820->map[i].addr + e820->map[i].size;
            if (tmp > mem_size)
            {
                mem_size = tmp;
            }
        }
    }
    memset(pages, 0, MAX_PAGES);
    for (volatile int32_t i = 0; i < KERNEL_PAGES; i++)// 将内核所在页标记为占用
    {
        pages[i] = 1;
    }
    // 建立物理内存的映射,物理内存向上取整
    map_range(TASK0_PML4, (uint64_t)VA(0), 0, 0, (mem_size + PAGE_SIZE - 1) / PAGE_SIZE);
}
```

完成线性映射后的效果

| 用途     | 虚拟地址                                  | 物理地址            | 大小 |
| -------- | ----------------------------------------- | ------------------- | ---- |
| 内核映射 | `0xffffffff80000000`-`0xffffffff84000000` | `0x0`-`0x4000000`   | 64MB |
| 恒等映射 | `0x0`-`0x200000`                          | `0x0`-`0x200000`    | 2MB  |
| 线性映射 | `0xffff888000000000`-`0xffff888140000000` | `0x0`-`0x140000000` | 4GB  |
|          |                                           |                     |      |

### i.用户程序

```c
//字符雨
typedef struct charpoint
{
    int8_t *buf; // 字符串
    int32_t x;   // 出生点
    int32_t y;
} charpoint;

int32_t num = 0;
charpoint cp[150];
uint32_t *fb;

void init()
{
    set_seed(get_ticks() + 1024);
    fb = get_buffer();//获得帧缓冲
    for (int32_t i = 0; i < CHARNUM; i++)
    {
        cp[i].buf = (int8_t *)umalloc(SCREENH / 16);//为每个cp申请存放字符的空间
        do//设置字符出生点
        {
            cp[i].x = rand(SCREENW - 8);
        } while (cp[i].x + 8 >= SCREENW);
        do
        {
            cp[i].y = rand(SCREENH - 16);
        } while (cp[i].y + 16 >= SCREENH);
    }
}
int8_t rand_chara()//获得随机字符
{
    return rand(95) + 32;
}
void cal_next()
{
    clear_screen();//每次绘制前清屏
    for (int i = 0; i < POINTNUM; i++)
    {
        cp[i].buf[num] = rand_chara();
    }
    num++;
    for (int i = 0; i < POINTNUM; i++)
    {
        for (int j = num - 1; j >= 0; j--)//先生成的字符在下面
        {
            if (cp[i].y + (num - j - 1) * 16 <= SCREENH - 16)
                color_printc1(cp[i].buf[j], cp[i].x, cp[i].y + (num - j - 1) * 16, 0, 255, 0, 0, 0, 0);//设置为绿色字体，黑色背景
        }
    }
}


while (num < 50)
{
    cal_next();
    display(fb);
    sleep(100);// 字符100毫秒下降一个
}
```

通过不断地`cal_next`和`display`，实现字符下落的效果

```c
int32_t *life, *nextlife; // 生命游戏也使用双缓冲思想
int32_t ps[8][2] = {{1, 1}, {1, -1}, {1, 0}, {-1, 0}, {-1, -1}, {-1, 1}, {0, 1}, {0, -1}}; // 周围八个格子
void set_cell(int32_t x, int32_t y, int32_t val)
{
    nextlife[x + y * CELLW] = val;
}
void switch_cell()
{
    memcpy(life, nextlife, sizeof(int32_t) * CELLW * CELLH);
}
void check(int32_t x, int32_t y) // 细胞存活规则
{
    int32_t num = 0;
    for (int32_t i = 0; i < 8; i++)
    {
        int32_t tx = x + ps[i][0], ty = y + ps[i][1];
        if (tx >= 0 && tx < CELLW && ty >= 0 && ty < CELLH)
        {
            num += life[ty * CELLW + tx];
        }
    }
    if (num < 2 || num > 3)
    {
        set_cell(x, y, 0);
    }
    else if (num == 3)
    {
        set_cell(x, y, 1);
    }
    else
    {
        set_cell(x, y, life[x + y * CELLW]);
    }
}
void draw_cell() // 绘制所有细胞
{
    for (int32_t i = 0; i < CELLH; i++)
    {
        for (int32_t j = 0; j < CELLW; j++)
        {
            if (life[i * CELLW + j] != 1)
            {
                draw_rect(j * CELL, i * CELL, (j + 1) * CELL, (i + 1) * CELL, 0xfd, 0xfd, 0xfd, 255);
            }
            else
            {
                draw_rect(j * CELL, i * CELL, (j + 1) * CELL, (i + 1) * CELL, 0, 0, 0, 255);
            }
        }
    }
}
void run_cell()
{
    for (int32_t i = 0; i < CELLH; i++)
    {
        for (int32_t j = 0; j < CELLW; j++)
        {
            check(j, i);
        }
    }
    switch_cell();
    draw_cell();
}

void lifegame_init() // 设置初始细胞布局
{
    life = (int32_t *)umalloc(sizeof(int32_t) * CELLW * CELLH);
    memset(life, 0, sizeof(int32_t) * CELLW * CELLH);
    nextlife = (int32_t *)umalloc(sizeof(int32_t) * CELLW * CELLH);
    memset(nextlife, 0, sizeof(int32_t) * CELLW * CELLH);
    set_cell(3, 1, 1);
    set_cell(3, 2, 1);
    set_cell(5, 2, 1);
    set_cell(1, 3, 1);
    set_cell(2, 4, 1);
    set_cell(4, 4, 1);
    set_cell(5, 4, 1);
    set_cell(6, 4, 1);
    set_cell(1, 5, 1);
    set_cell(3, 6, 1);
    set_cell(5, 6, 1);
    set_cell(3, 7, 1);
}
void rand_cell() // 随机生成细胞
{
    int32_t nt = rand(CELLH * CELLW - 1);
    while (nt <= 8 || nt > CELLH * CELLW)
        nt = rand(CELLH * CELLW - 1);
    for (int32_t i = 0; i < nt; i++)
    {
        int32_t x = rand(CELLW), y = rand(CELLH);
        set_cell(x, y, 1);
    }
    switch_cell();
}

clear_screen1();//白背景清屏
lifegame_init();
set_cell(3, 4, 1);//把这个放到init里面就会出现bug，可能是优化导致的问题
switch_cell();
draw_cell();
display(fb);
while (1)
{
	run_cell();
	display(fb);
	cha = getchar();// 获取按键信息
	if (cha == ' ')
	{
		rand_cell();
	}
	sleep(500);//每500毫秒更新细胞存活状态
}
```

## 5.运行效果

### a.内核模式下的图像绘制测试

![7f92d4769b841ca7313169973a2379f3_720](.\picture\7f92d4769b841ca7313169973a2379f3_720.png)

### b.用户程序里的字符雨

![247c9197f4a0d0b55f9fb633c35900a9_720](.\picture\247c9197f4a0d0b55f9fb633c35900a9_720.png)

### c. 用户程序的提示信息

![b65664e27e1fe0baee8e889e8c3a5843](.\picture\b65664e27e1fe0baee8e889e8c3a5843.png)

单击空格后进入生命游戏界面

### d.生命游戏初始的一个特定图形

![c227febd0b1cf3351cb05325e6945f55](.\picture\c227febd0b1cf3351cb05325e6945f55.png)

单击空格键生成随机细胞分布

### e.随机分布的一个细胞

![ef47d7faac29f32b430de161b41b9665](.\picture\ef47d7faac29f32b430de161b41b9665.png)

细胞随着时间的推进会根据规则生长凋亡

### f.上一个随机分布细胞的最终布局

![8f9ec598c37b3eba3c68c660ddfc50d6](.\picture\8f9ec598c37b3eba3c68c660ddfc50d6.png)

## 6.参考资料

### a. [穿越操作系统迷雾 ](https://book.douban.com/subject/36560814/)

![穿越操作系统迷雾](.\picture\s34644418.jpg)

### b. [xv6操作系统]([mit-pdos/xv6-public: xv6 OS (github.com)](https://github.com/mit-pdos/xv6-public))

​	[xv6 中文文档 ](https://th0ar.gitbooks.io/xv6-chinese/content/)

### c. [教你用代码演奏天空之城](https://blog.csdn.net/weixin_45697774/article/details/106868403)

### d.[生命游戏](https://conwaylife.com/wiki/Main_Page)

### e. [哔哩哔哩-操作系统实现-踌躇月光](https://space.bilibili.com/491131440/channel/collectiondetail?sid=146887)

### f .[ASCII字符艺术生成器]([Text to ASCII Art Generator (TAAG) (patorjk.com)](https://www.patorjk.com/software/taag/#p=display&c=bash&f=Alpha&t=qiat))

