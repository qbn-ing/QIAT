#include "atkbd.h"

static int8_t buffer[KBD_BUFFER_SIZE];
static cqueue_t kbd_buf;
uint8_t CapsLock; // 这些键是否被按下
uint8_t precode, curcode;
struct semaphore *kbdLock;
uint8_t keyMap[] = {
    [0x1E] = 'a',
    [0x30] = 'b',
    [0x2E] = 'c',
    [0x20] = 'd',
    [0x12] = 'e',
    [0x21] = 'f',
    [0x22] = 'g',
    [0x23] = 'h',
    [0x17] = 'i',
    [0x24] = 'j',
    [0x25] = 'k',
    [0x26] = 'l',
    [0x32] = 'm',
    [0x31] = 'n',
    [0x18] = 'o',
    [0x19] = 'p',
    [0x10] = 'q',
    [0x13] = 'r',
    [0x1F] = 's',
    [0x14] = 't',
    [0x16] = 'u',
    [0x2F] = 'v',
    [0x11] = 'w',
    [0x2D] = 'x',
    [0x15] = 'y',
    [0x2C] = 'z',
    [0x0B] = '0',
    [0x02] = '1',
    [0x03] = '2',
    [0x04] = '3',
    [0x05] = '4',
    [0x06] = '5',
    [0x07] = '6',
    [0x08] = '7',
    [0x09] = '8',
    [0x0A] = '9',
    [0X39] = ' ',
    [0x35] = '/',
    [0x34] = '.',
};
void kbd_init()
{
    CapsLock = 0;
    kbdLock = malloc(sizeof(struct semaphore));
    kbdLock->counter = 0; //
    queue_init(&kbd_buf, buffer, KBD_BUFFER_SIZE);
    curcode = 0;
}
void kbd_work()
{
    curcode = in(KBDATAP);         // 读取键盘输入
    if (precode + 0x80 == curcode) // 这些键抬起
    {
        curcode = 0;
        precode = 0;
    }
    precode = curcode;

    if (keyMap[curcode] != 0)
    {
        if (CapsLock)
        {
            outb(0x3f8, toUpper(keyMap[curcode]));
            // print(toUpper(keyMap[curcode]));
            queue_push(&kbd_buf, toUpper(keyMap[curcode]));
        }
        else
        {
            outb(0x3f8, (keyMap[curcode]));
            // print(keyMap[curcode]);
            queue_push(&kbd_buf, keyMap[curcode]);
        }
    }
    signal(kbdLock);
    // printn();
    if (curcode == ENTER || curcode == BACKSPACE || curcode == CAPSLOCK) // 只处理这些功能键
    {
        if (curcode == CAPSLOCK)
        {
            CapsLock = !CapsLock;
        }
        return;
    }
}
int8_t get_kbd()
{
    //
    if (queue_empty(&kbd_buf))
    {
        return 0;
    }
    int8_t q = queue_pop(&kbd_buf);
    sprint("the char is:");
    hprint((int64_t)q);
    return q;
}
int64_t gets_kbd(int8_t *buf, uint32_t count)
{

    sprint("get!!!\n\r");
    for (int32_t i = 0; i < count; i++)
    {
        hprint(i);
        wait(kbdLock);
        buf[i] = get_kbd();
    }
    hprint(buf[0]);
    return count;
}