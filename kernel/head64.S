//放到0x100000处
#include"../include/mmu.h"
.text
.code64
.globl _start
.globl gdt
.equ task0_stack,0xfff8 // 任务0的栈,放到页表之前
_start:
  //hlt
  lgdt gdtr
  //hlt
  lidt idtr

  // 设置段寄存器
  mov $SEG_KCODE, %ax
  mov %ax, %ds
  mov %ax, %es
  mov $0 , %ax
  mov %ax, %fs
  mov %ax, %gs
  mov %ax, %ss

  // 设置栈
  mov $task0_stack, %rsp

  pushq $main64
  ret
  hlt

gdt:
  .quad 0x0000000000000000  // 保留 
  .quad 0x00209a0000000000  // 内核代码段 
  .quad 0x0000920000000000  // 内核数据段
  .quad 0x0000000000000000  // 32位用户代码段 
  .quad 0x0000f20000000000  // 用户数据段
  .quad 0x0020fa0000000000  // 64位用户代码段
  .fill 64 - 6, 8, 0
gdt_end:
gdtr:
  .word gdt_end - gdt
  .quad gdt
idtr:
  .word 16*256
  .quad idt


