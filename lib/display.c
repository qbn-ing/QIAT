#include "../include/display.h"
#include "../include/asm.h"
#include "../include/string.h"
#include "include/math.h"

uint32_t *fb_addr;
uint8_t *chars;
inline uint32_t *get_buffer()
{
    fb_addr = (uint32_t *)umalloc(SCREENH * SCREENW * DEPTH);
    hprint((int64_t)fb_addr);
    clear_screen();
    setChar_bits((uint8_t *)0xffffff000);
    return fb_addr;
}
inline void draw_point(int32_t x, int32_t y, int8_t r, int8_t g, int8_t b, int8_t a)
{
    volatile uint32_t *tmpfb = fb_addr + SCREENW * y + x;
    *((int8_t *)tmpfb + 0) = b;
    *((int8_t *)tmpfb + 1) = g;
    *((int8_t *)tmpfb + 2) = r;
    *((int8_t *)tmpfb + 3) = a;
}
void draw_heart(int32_t x, int32_t y, int32_t rr, int8_t r, int8_t g, int8_t b, int8_t a)
{
    int32_t stx = x - rr, edx = x + rr, sty = y - rr, edy = y + rr;
    if (stx < 0)
        stx = 0;
    if (edx >= SCREENW)
        edx = SCREENW - 1;
    if (sty < 0)
        sty = 0;
    if (edy >= SCREENH)
        edy = SCREENH - 1;
    for (int32_t i = stx; i <= edx; i++)
    {
        for (int32_t j = sty; j <= edy; j++)
        {
            int64_t xx = (i - rr);
            int64_t yy = (rr - j);
            int64_t x2 = xx * xx;
            int64_t y2 = yy * yy;
            int64_t absx = xx;
            if (absx <= 0)
                absx *= -1;
            int64_t term1 = x2 - absx * yy + y2;
            int64_t term2 = rr * rr;
            if (term1 - term2 <= 0)
            {
                draw_point(i, j, r, g, b, a);
            }
        }
    }
}
inline void draw_line(volatile int32_t sx, volatile int32_t sy, volatile int32_t ex, volatile int32_t ey, volatile int8_t r, volatile int8_t g, volatile int8_t b, volatile int8_t a)
{

    if (sx < 0 || sx >= SCREENW || sy < 0 || sy >= SCREENH || ex < 0 || ex >= SCREENW || ey < 0 || ey >= SCREENH)
    {
        sprint("error!!!");
        return;
    }
    // sprint("from:");
    // dprint(sx);
    // sprint(",");
    // dprint(sy);
    // sprint("to ");
    // dprint(ex);
    // sprint(",");
    // dprint(ey);
    // printn();
    if (sx == ex)
    {
        for (volatile int32_t i = sy; i <= ey; i++)
        {
            draw_point(sx, i, r, g, b, a);
        }
        return;
    }
    if (sy == ey)
    {
        for (volatile int32_t i = sx; i <= ex; i++)
        {
            draw_point(i, sy, r, g, b, a);
        }
    }
    int32_t dx = ex - sx;
    int32_t dy = ey - sy;
    int32_t x = sx;
    int32_t y = sy;
    int32_t p = 2 * dy - dx;

    while (x < ex)
    {
        if (p >= 0)
        {
            draw_point(x, y, r, g, b, a);
            y++;
            p += 2 * dy - 2 * dx;
        }
        else
        {
            draw_point(x, y, r, g, b, a);
            p += 2 * dy;
        }
        x++;
    }
}
inline void draw_rect(int32_t ltx, int32_t lty, int32_t rbx, int32_t rby, int8_t r, int8_t g, int8_t b, int8_t a)
{
    if (ltx > rbx)
    {
        int32_t tmp = ltx;
        ltx = rbx;
        rbx = tmp;
    }
    if (lty > rby)
    {
        int32_t tmp = lty;
        lty = rby;
        rby = tmp;
    }
    for (volatile int32_t tx = ltx; tx <= rbx; tx++)
    {
        draw_line(tx, lty, tx, rby, r, g, b, a);
    }
}
inline void setChar_bits(uint8_t *charBit)
{
    chars = charBit;
}
inline int64_t switch_buffer(uint32_t *buffer)
{
    memcpy(fb_addr, buffer, SCREENW * SCREENH * DEPTH);
    return 0;
}
inline void scroll_screen()
{
    uint32_t *fb = fb_addr;
    int32_t pixels_per_line = SCREENW; // 假设每个像素占用4字节

    memcpy(fb, fb + pixels_per_line * 16, pixels_per_line * (SCREENH - 16) * 4);

    // 清空底部的新行
    memset(fb + (SCREENH - 16) * pixels_per_line, 0, pixels_per_line * 16 * 4);
}
inline void clear_screen()
{
    memset(fb_addr, 0, SCREENW * SCREENH * 4);
}
inline void clear_screen1()
{
    memset(fb_addr, 0xff, SCREENW * SCREENH * 4);
}
void set_fb(uint64_t addr)
{
    fb_addr = (uint32_t *)addr;
}
void color_printc1(int8_t cha, int32_t x, int32_t y, int8_t rf, int8_t gf, int8_t bf, int8_t rb, int8_t gb, int8_t bb)
{
    if (y > SCREENH - 16)
    {
        scroll_screen();
        return;
        y -= 16;
    }
    sprint("x==");
    dprint(x);
    sprint("y==");
    dprint(y);
    sprint("char==");
    hprint(cha);
    color_printc(cha, x, y, fb_addr, rf, gf, bf, rb, gb, bb);
}

void color_prints1(const int8_t *str, int32_t x, int32_t y, int8_t *rf, int8_t *gf, int8_t *bf, int8_t *rb, int8_t *gb, int8_t *bb)
{
    if (y > SCREENH - 16)
    {
        scroll_screen();
        y -= 16;
    }
    color_prints(str, x, y, fb_addr, rf, gf, bf, rb, gb, bb);
}
void color_printc(int8_t cha, int32_t x, int32_t y, uint32_t *fb, int8_t rf, int8_t gf, int8_t bf, int8_t rb, int8_t gb, int8_t bb)
{

    int32_t index = cha - ' ';
    uint32_t *tmpfb = fb + SCREENW * y + x;
    if (index < 0)
    {
        sprint("wrong!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\rindex<0 now\n\r");
        hlt();
    }
    sprint("fb==");
    hprint((int64_t)tmpfb);
    // sprint("print:");
    // print(cha);
    // hprint(cha);
    // printn();
    sprint("x==");
    dprint(x);
    sprint("y==");
    dprint(y);
    for (volatile int32_t i = 0; i < 16; i++)
    {
        if (tmpfb >= fb + SCREENW * SCREENH)
        {
            // sprint("error!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\r");
            return;
        }
        int8_t nc = *(chars + index * 16 + i);
        for (volatile int32_t j = 7; j >= 0; j--)
        {
            if ((nc >> j) & 0x1)
            {
                *((int8_t *)tmpfb + 0) = bf;
                *((int8_t *)tmpfb + 1) = gf;
                *((int8_t *)tmpfb + 2) = rf;           // r
                *((int8_t *)tmpfb + 3) = (int8_t)0x00; //
            }
            else
            {
                *((int8_t *)tmpfb + 0) = bb; // b
                *((int8_t *)tmpfb + 1) = gb;
                *((int8_t *)tmpfb + 2) = rb;
                *((int8_t *)tmpfb + 3) = (int8_t)0xff; //
            }
            tmpfb += 1;
        }
        tmpfb = tmpfb + SCREENW - 8;
        // sprint("tfb==");
        // hprint((int64_t)tmpfb);
    }
    sprint("char over!\n\r");
}
void color_prints(const int8_t *str, int32_t x, int32_t y, uint32_t *fb, int8_t *rf, int8_t *gf, int8_t *bf, int8_t *rb, int8_t *gb, int8_t *bb)
{
    int32_t xP = x;
    for (volatile int32_t k = 0; k < strlen((int8_t *)str); k++)
    {
        if (str[k] == '\n')
        {
            y += 16;
            x = xP;
            continue;
        }
        if (str[k] == '\r')
        {
            x = 0;
            continue;
        }
        color_printc(str[k], x, y, fb, bf[k], gf[k], rf[k], bb[k], gb[k], rb[k]);
        x += 8;
    }
}
inline void printc(int8_t cha, int32_t x, int32_t y, uint32_t *fb, uint8_t con)
{
    // sprint("printc\n\r");
    if (con == 0)
        color_printc(cha, x, y, fb, 0xff, 0xff, 0xff, 0x01, 0x01, 0x01);
    else if (con == 1)
        color_printc(cha, x, y, fb, 0x00, 0x00, 0x00, 0xf0, 0xf0, 0xf0);
}
void prints(const int8_t *str, int32_t x, int32_t y, uint32_t *fb, uint8_t con)
{
    int32_t xP = x;
    // printn();
    // dprint(strlen((int8_t *)str));
    for (volatile int32_t k = 0; k < strlen((int8_t *)str); k++)
    {
        if (str[k] == '\n')
        {
            y += 16;
            x = xP;
            continue;
        }
        if (str[k] == '\r')
        {
            x = 0;
            continue;
        }
        printc(str[k], x, y, fb, con);
        // sprint("0k!\n\r");
        x += 8;
    }
}
inline void printh(uint64_t x, int32_t xPosition, int32_t yPosition, uint32_t *fb)
{
    // sprint("\n\rstartprinth\n\r");
    uint64_t y = x, tp = 1;
    if (x == 0)
    {
        prints1("0x0", xPosition, yPosition, 1);
        return;
    }
    y >>= 4;
    while (y)
    {
        tp <<= 4;
        y >>= 4;
    }
    // sprint("001\n\r");
    prints("0x", xPosition, yPosition, fb, 1);
    // sprint("0x");
    xPosition += 16;
    while (tp)
    {
        uint8_t t = x / tp;
        char cc;
        if (t < 10)
            cc = ('0' + t);
        else
            cc = ('a' + t - 10);
        printc1(cc, xPosition, yPosition, 0);
        xPosition += 8;
        x %= tp;
        tp >>= 4;
    }
}
void printc1(int8_t cha, int32_t x, int32_t y, uint8_t con)
{
    if (y > SCREENH - 16)
    {
        scroll_screen();
        y -= 16;
    }
    printc(cha, x, y, fb_addr, con);
}
void prints1(const int8_t *str, int32_t x, int32_t y, uint8_t con)
{
    hprint(strlen((int8_t *)str));
    if (y > SCREENH - 16)
    {
        scroll_screen();
        y -= 16;
    }
    prints(str, x, y, fb_addr, con);
    // sprint("ko!\n\r");
}
void printh1(uint64_t x, int32_t xPosition, int32_t yPosition)
{
    if (yPosition > SCREENH - 16)
    {
        scroll_screen();
        yPosition -= 16;
    }
    printh(x, xPosition, yPosition, fb_addr);
}
int32_t getX()
{
    uint32_t *fb_end = fb_addr + SCREENW * SCREENH * 4;
    while (fb_end != fb_addr && *fb_end == 0)
    {
        fb_end--;
    }
    int32_t x = (int32_t)(fb_end - fb_addr) % SCREENW;
    x = (x + 8) / 8 * 8;
    return x;
}
int32_t getY()
{
    uint32_t *fb_end = fb_addr + SCREENW * SCREENH * 4;
    while (fb_end != fb_addr && *fb_end == 0)
    {
        fb_end--;
    }
    volatile int32_t y = (int32_t)(fb_end - fb_addr + SCREENW) / SCREENW;
    y = (y + 16) / 16 * 16;
    return y;
}
void printd(int64_t x, int32_t xPosition, int32_t yPosition)
{
    int64_t tmp = x, y = 1;
    if (x == 0)
    {
        printc1('0', xPosition, yPosition, 0);
    }
    x /= 10;
    while (x)
    {
        y = y * 10;
        x /= 10;
    }
    while (y)
    {
        char cc;
        cc = '0' + tmp / y;
        printc1(cc, xPosition, yPosition, 0);
        xPosition += 8;
        tmp %= y;
        y /= 10;
    }
}
void printd1(int64_t x, int32_t xPosition, int32_t yPosition)
{
    if (yPosition > SCREENH - 16)
    {
        scroll_screen();
        yPosition -= 16;
    }
    printd(x, xPosition, yPosition);
}
// uint64_t getVfb()
// {
//     // 获得一个虚拟bf
//     uint64_t size = SCREENH * SCREENW * DEEP; // 5814000 接近6MB
// }
// little toys
