#include "../include/asm.h"
#include "../lib/std.h"
#include "../include/display.h"
#include "../include/string.h"
#include "../drivers/speaker.h"
int main()
{
    int32_t pai = 400;
    int32_t pai2 = 200;
    int32_t pai3 = 100;

    int32_t ting = 120;
    while (1)
    {
        beep(do, pai);
        beep(re, pai);
        beep(mi, pai);
        beep(do, pai);
        sleep(ting);
        beep(do, pai);
        beep(re, pai);
        beep(mi, pai);
        beep(do, pai);
        sleep(ting);
        beep(mi, pai);
        beep(fa, pai);
        beep(so, pai * 2);
        sleep(ting);
        beep(mi, pai);
        beep(fa, pai);
        beep(so, pai * 2);
        sleep(ting);
        beep(so, pai2);
        beep(la, pai3);
        beep(so, pai2);
        beep(fa, pai3);
        beep(mi, pai);
        beep(do, pai);
        sleep(ting);
        beep(so, pai2);
        beep(la, pai3);
        beep(so, pai2);
        beep(fa, pai3);
        beep(mi, pai);
        beep(do, pai);
        sleep(ting);
        beep(do, pai);
        beep(so, pai);
        beep(do, pai * 2);
        sleep(ting);
        beep(do, pai);
        beep(so, pai);
        beep(do, pai * 2);
        sleep(ting);
    }
}