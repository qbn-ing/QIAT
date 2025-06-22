#pragma once
#include "../include/types.h"

extern int64_t sleep(int64_t ms);      // sleep的系统调用，在syscall.S中实现
extern int32_t display(uint32_t *vbf); // 交换缓冲区的系统调用
extern int64_t umalloc(uint32_t size); // 交换缓冲区的系统调用
extern int64_t get_ticks();
extern int64_t gets(int8_t *buf, uint32_t count); // 从键盘读取字符串,暂时还有问题
extern int64_t getchar();                         // 从键盘读取字符,不阻塞进程，无字符返回0