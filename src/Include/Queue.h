#pragma once

#include <Macros.h>
#include <String.h>
#include <stddef.h>
#include <stdint.h>


struct Queue
{
	uint8_t *items;
	size_t    size;
	size_t   nmemb;
	size_t    head;
	size_t    tail;
};


static INLINE int QueueFull(struct Queue *q)
{
	return q->head == (q->tail + 1) % q->nmemb;
}

static INLINE int QueueEmpty(struct Queue *q)
{
	return q->head == q->tail;
}

static INLINE int QueueSubmit(struct Queue *q, void *item)
{
	if(q->head == (q->tail + 1) % q->nmemb) return 0;

	if(item != NULL)
		memcpy(&q->items[q->size * q->tail], item, q->size);

	q->tail = (q->tail + 1) % q->nmemb;

	return 1;
}

static INLINE int QueueConsume(struct Queue *q, void *item)
{
	if(q->head == q->tail) return 0;

	if(item != NULL)
		memcpy(item, &q->items[q->size * q->head], q->size);

	q->head = (q->head + 1) % q->nmemb;

	return 1;
}
