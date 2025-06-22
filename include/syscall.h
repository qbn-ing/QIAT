#pragma once
// 系统调用号
#define SYS_sleep 1
#define SYS_display 2
#define SYS_umalloc 3
#define SYS_get_ticks 4
#define SYS_gets 5
#define SYS_getchar 6
void syscall_handler();
void syscall_init();
