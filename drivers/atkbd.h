#pragma once
#include "../include/types.h"
#include "../include/asm.h"
#include "../mm/mm.h"
#include "../include/sched.h"
#include "../include/string.h"
#include "../include/queue.h"

#define KBSTATP 0x64 // 键盘控制器状态端口
#define KBS_DIB 0x01 // 键盘buffer
#define KBDATAP 0x60 // 键盘数据
#define KBD_BUFFER_SIZE 64
#define BACKSPACE 0x0e
#define ENTER 0x1c
#define CAPSLOCK 0x3a
#define SPACE 0x39
#define NO 0

void kbd_work();
void kbd_init();
int8_t get_kbd();
int64_t gets_kbd(int8_t *buf, uint32_t count);