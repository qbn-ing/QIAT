#pragma once
#include "types.h"
// #include "chara.h"
#include "../mm/mm.h"
#include "../lib/std.h"
// #include <stdarg.h> //支持可变参数
#define SCREENW 1440
#define SCREENH 900
#define DEPTH 4

extern uint32_t *fb_addr;
void scroll_screen();

uint32_t *get_buffer();
int64_t switch_buffer(uint32_t *buffer);

void set_fb(uint64_t addr);
void clear_screen();
void clear_screen1();
void setChar_bits(uint8_t *charBit);
void color_printc(int8_t cha, int32_t x, int32_t y, uint32_t *fb, int8_t rf, int8_t gf, int8_t bf, int8_t rb, int8_t gb, int8_t bb);
void color_prints(const int8_t *str, int32_t x, int32_t y, uint32_t *fb, int8_t *rf, int8_t *gf, int8_t *bf, int8_t *rb, int8_t *gb, int8_t *bb);
void color_printc1(int8_t cha, int32_t x, int32_t y, int8_t rf, int8_t gf, int8_t bf, int8_t rb, int8_t gb, int8_t bb);
void color_prints1(const int8_t *str, int32_t x, int32_t y, int8_t *rf, int8_t *gf, int8_t *bf, int8_t *rb, int8_t *gb, int8_t *bb);

void printc(int8_t cha, int32_t x, int32_t y, uint32_t *fb, uint8_t con);
void prints(const int8_t *str, int32_t x, int32_t y, uint32_t *fb, uint8_t con);
void printh(uint64_t x, int32_t xPosition, int32_t yPosition, uint32_t *fb);
void printc1(int8_t cha, int32_t x, int32_t y, uint8_t con);
void prints1(const int8_t *str, int32_t x, int32_t y, uint8_t con);
void printh1(uint64_t x, int32_t xPosition, int32_t yPosition);
void printd(int64_t x, int32_t xPosition, int32_t yPosition);

void printd1(int64_t x, int32_t xPosition, int32_t yPosition);
int32_t getX();
int32_t getY();
void draw_point(int32_t x, int32_t y, int8_t r, int8_t g, int8_t b, int8_t a);
void draw_line(int32_t sx, int32_t sy, int32_t ex, int32_t ey, int8_t r, int8_t g, int8_t b, int8_t a);
void draw_rect(int32_t ltx, int32_t lty, int32_t rbx, int32_t rby, int8_t r, int8_t g, int8_t b, int8_t a);
void draw_heart(int32_t x, int32_t y, int32_t rr, int8_t r, int8_t g, int8_t b, int8_t a);
