// 0x7e00
#include "../include/mmu.h"

.text
.code16
.globl start
start:
    

      # 设置光标位置
    mov $0x02, %ax      # 功能号02h
    mov $0x00, %bx      # 页号
    mov $0x00, %dx      # dh = 行号, dl = 列号
    int $0x10            # 调用BIOS中断


    # 显示字符串
    movw $0x1301, %ax    # 功能号13h，子功能号01h
    movw $0x000f, %bx    # 属性（黑底白字）
    movw $0x000f, %cx    # 字符串长度
    leaw StartLoaderMessage, %bp # 字符串地址
    int $0x10            # 调用BIOS中断
    # 开启A20
    in $0x92,%al
    or $0x02,%al
    out %al,$0x92


    # 读 e820获取内存信息
    mov $E820_ADDR, %di
    xorl %ebx, %ebx
    e820_rd_entry:
        movl $0xe820, %eax   
        movl $20, %ecx 
        movl $0x534D4150, %edx //不加smap签名CF会=1
        int $0x15
        add $20, %di
        incb E820InFo_ADDR
        cmp $0, %ebx   
        jne e820_rd_entry


    #设定143 800*60 32的显示模式 180 1440*900
    mov $0x4f02, %ax
    mov $0x4180, %bx
    int $0x10
    # 获取VBE模式信息
    movw $0x4F01, %ax
    movw $0x4180, %cx  # 同样的模式号
    mov $VBEiInfo_ADDR, %di  # 指向一个内存缓冲区
    int $0x10
    
    # 初始化 8259A
    # ICW1 
    mov $0x11, %al #使用ICW4
    mov $0x20, %dx
    out %al, %dx
    # ICW2 
    mov $0x20, %al #“通常情况下“
    mov $0x21, %dx
    out %al, %dx
    # ICW3 
    mov $0x04, %al #IR2接从片
    mov $0x21, %dx
    out %al, %dx
    # ICW4 
    mov $0x01, %al # 8086模式
    mov $0x21, %dx
    out %al, %dx
    
    
    movw $0x7e00, %sp

  # 设置gdt
    lgdt gdtr
    
    # 开启保护模式
    mov %cr0, %eax
    or $0x1, %eax
    mov %eax, %cr0

    call boot32
gdtr:
  .word gdt_end - gdt
  .long gdt
gdt:
  .quad 0x0000000000000000
  .quad 0x00c09a00000007ff
  .quad 0x00c09200000007ff
gdt_end:
StartLoaderMessage: .asciz "Start Loader.\n\r"
StartGetMemInfo: .asciz "Start Get Mem Info.\n\r"
GetMemInfoDone: .asciz "Get Mem Info Done.\n\r"
StartGetVbeInfo: .asciz "Start Get Vbe Info.\n\r"
GetVbeInfoDone: .asciz "Get Vbe Info Done.\n\r"
StartSetVideoMode: .asciz "Start Set Video Mode.\n\r"

StartSetGdt: .asciz "Start Set Gdt.\n\r"
StartSetIdt: .asciz "Start Set Idt.\n\r"
