//放到0xa000处
#include"../include/mmu.h"
.text
.code32
.globl start32
start32:
 # 设置其它寄存器的段选择子
  movw $(SEG_KDATA), %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %ss
  movw $0, %ax  # 0表示不准备用
  movw %ax, %fs
  movw %ax, %gs
  

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

  // 设置临时GDT
  lgdt gdtr32

  // 启用PAE x86的物理地址扩展
  xorl %eax, %eax
  btsl $5, %eax
  movl %eax, %cr4

  // 设置CR3页表地址
  movl  $0x10000, %eax
  movl  %eax, %cr3

  // 设置EFER
  movl  $0xc0000080, %ecx
  rdmsr

  // 64位长模式
  btsl  $8, %eax
  // 启用 syscall/sysret
  btsl  $0, %eax
  wrmsr

  mov %cr0, %eax
  btsl $31, %eax  // 开启分页
  // 启用保护模式
  btsl $0, %eax
  movl %eax, %cr0

  call head64

gdt32:
  .quad 0x0000000000000000
  .quad 0x00209a0000000000
  .quad 0x0000920000000000
gdt_end32:

gdtr32:
  .word gdt_end32-gdt32
  .long gdt32
  