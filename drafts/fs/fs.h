#pragma once
#include "../include/types.h"
#define UNK 0
#define FOLDER 1
#define TXT 2
#define EXE 3
#define AUD 4
#define KBD 5

struct fcb // 共 3+1+16+4+8=32 字节
{
    int8_t magic[3]; // 魔数 "FSB"
    int8_t type;     // 文件类型
    int8_t name[16]; // 文件名
    int32_t size;    // 文件大小
    int64_t start;   // 文件起始位置
} __attribute__((packed));
struct fcblist // 文件表
{
    int8_t magic[3];     // 魔数 "FSL"
    struct fcb fcbs[10]; // 文件控制块
} __attribute__((packed));
// 可能不会超过十个文件
// 可能得新建一个makefile