#include "include/syscall.h"
#include "include/interrupt.h"

#define MSR_STAR 0xc0000081
#define MSR_LSTAR 0xc0000082
#define MSR_SYSCALL_MASK 0xc0000084
#define RF_IF 0x00000200

typedef int64_t (*fn_ptr)();

extern int64_t sys_sleep(int32_t time);
extern int64_t swap_buffers(uint32_t *buffer);
extern int64_t sys_malloc_big(int32_t size);
extern int64_t sys_get_ticks();
extern int64_t gets_kbd(int8_t *buf, uint32_t count);
extern int64_t get_kbd();
fn_ptr syscall_table[] = {

    [SYS_sleep] sys_sleep,
    [SYS_display] swap_buffers,
    [SYS_umalloc] sys_malloc_big,
    [SYS_get_ticks] sys_get_ticks,
    [SYS_gets] gets_kbd,
    [SYS_getchar] get_kbd,
};

void syscall_init() // 设置MSR寄存器
{
    uint64_t star_val = (uint64_t)SEG_U32CODE << 48 | (uint64_t)SEG_KCODE << 32;
    uint64_t syscall_entry = (uint64_t)syscall_handler;
    uint64_t syscall_mask = RF_IF;

    __asm__("wrmsr" : : "c"(MSR_STAR), "a"((uint32_t)star_val), "d"(star_val >> 32));
    __asm__("wrmsr" : : "c"(MSR_LSTAR), "a"((uint32_t)syscall_entry), "d"(syscall_entry >> 32));
    __asm__("wrmsr" : : "c"(MSR_SYSCALL_MASK), "a"((uint32_t)syscall_mask), "d"(syscall_mask >> 32));
}
