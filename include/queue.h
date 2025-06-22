#pragma once
#include "../include/types.h"

typedef struct cqueue
{
    int8_t *buf;     // 传入一个数组作为队列
    uint32_t length; // 必须传入长度，循环队列
    uint32_t head;   // 出队
    uint32_t tail;   // 入队
} cqueue_t;          // 循环队列

void queue_init(cqueue_t *q, int8_t *buf, uint32_t length);

int8_t queue_pop(cqueue_t *q);

void queue_push(cqueue_t *q, int8_t byte);

int8_t queue_empty(cqueue_t *q);

int8_t queue_full(cqueue_t *q);