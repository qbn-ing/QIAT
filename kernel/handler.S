.text
.code64
.globl alltraps


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

    call trap
    

    



.globl trapret
trapret:
    #恢复寄存器
    //hlt
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
    
    sti
    iretq

.globl ret_from_kernel
ret_from_kernel:
 #恢复寄存器
    mov $0x23,%rax
    movw %ax,%ds
    movw %ax,%es
    movw %ax,%fs
    movw %ax,%gs
    //hlt
    sti
    iretq

.globl default_handler
.globl pf_handler
.globl timer_handler
.globl kbd_handler
.globl ide_handler
.globl syscall_handler
.globl ox76_handler//一个莫名其妙的硬件中断
.globl illop_handler
default_handler: //用来接收不处理的中断和异常 
    cli
    pushq $128 //错误码
    pushq $255 //中断号
    jmp alltraps
illop_handler:
    cli
    pushq $0 //错误码
    pushq $0x6 //中断号
    jmp alltraps
    
ox76_handler:
    cli
    pushq $0 //错误码
    pushq $0x76 //中断号
    jmp alltraps
pf_handler:
    cli
    pushq $0 //错误码
    pushq $14 //中断号
    jmp alltraps

timer_handler:
    //cli
    pushq $0 //错误码
    pushq $32 //中断号
    jmp alltraps
kbd_handler:
    cli
    pushq $0 //错误码
    pushq $33 //中断号
    jmp alltraps

ide_handler:
    cli
    pushq $0 //错误码
    pushq $46 //中断号
    jmp alltraps

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

    alltrapsa

    call *syscall_table(, %rax, 8) //查找系统调用表


    trapsreta
    popq %rsp
    sysretq

