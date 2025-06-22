#pragma once
#include "../include/mmu.h"
#include "../include/types.h"
#include "../mm/mm.h"
#include "../include/string.h"

#define SCREENW 1440
#define SCREENH 900
#define DEPTH 4

// 获得显卡的帧缓冲地址,并提供交换缓冲的接口
extern uint64_t vbf;

void vga_init();

int64_t swap_buffers(uint32_t *buffer);
