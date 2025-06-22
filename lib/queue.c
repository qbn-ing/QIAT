#include "../include/queue.h"

inline static uint32_t queue_next(cqueue_t *q, uint32_t index)
{
    return (index + 1) % q->length;
}
void queue_init(cqueue_t *q, int8_t *buf, uint32_t length)
{
    q->buf = buf;
    q->length = length;
    q->head = 0;
    q->tail = 0;
}

int8_t queue_pop(cqueue_t *q)
{
    if (queue_empty(q))
        return -1;
    int8_t byte = q->buf[q->head];
    q->head = queue_next(q, q->head);
    return byte;
}

void queue_push(cqueue_t *q, int8_t byte)
{
    while (queue_full(q))
    {
        queue_pop(q);
    }
    q->buf[q->tail] = byte;
    q->tail = queue_next(q, q->tail);
}

int8_t queue_empty(cqueue_t *q)
{
    return q->head == q->tail;
}

int8_t queue_full(cqueue_t *q)
{
    return queue_next(q, q->tail) == q->head;
}