# 引导程序,放到0x7c00处
.text
.code16
.globl start
start:
  # 关闭中断
  cli

  # 设置段寄存器
  xorw %ax, %ax
  movw %ax, %ds
  movw %ax, %es

  movw $0x7c00, %sp
  call    bootloader
  

