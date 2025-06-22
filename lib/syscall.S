.code64
#define SYS_sleep 1
#define SYS_display 2
#define SYS_umalloc 3
#define SYS_get_ticks 4
#define SYS_gets 5
#define SYS_getchar 6


#define SYSCALL(name) \
  .globl name; \
  name: \
    movq $SYS_ ## name, %rax; \
    syscall ;\
    ret ;\

SYSCALL(sleep)
SYSCALL(display)
SYSCALL(umalloc)
SYSCALL(get_ticks)
SYSCALL(gets)
SYSCALL(getchar)