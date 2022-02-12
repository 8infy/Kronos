#pragma once

#include <String.h>
#include <Types.h>


struct Queue
{
	void  *items;
	size_t  size;
	size_t nmemb;
	size_t  head;
	size_t  tail;
};


static inline size_t QueueCount(struct Queue *q)
{
	return q->head <= q->tail ? q->tail - q->head : q->tail + q->nmemb - q->head;
}

static inline int QueueFull(struct Queue *q)
{
	return q->head == (q->tail + 1) % q->nmemb;
}

static inline int QueueEmpty(struct Queue *q)
{
	return q->head == q->tail;
}

static inline int QueueSubmit(struct Queue *q, void *item)
{
	if(q->head == (q->tail + 1) % q->nmemb) return 0;

	if(item != NULL)
		memcpy(&((uint8_t *)q->items)[q->size * q->tail], item, q->size);

	q->tail = (q->tail + 1) % q->nmemb;

	return 1;
}

static inline int QueueConsume(struct Queue *q, void *item)
{
	if(q->head == q->tail) return 0;

	if(item != NULL)
		memcpy(item, &((uint8_t *)q->items)[q->size * q->head], q->size);

	q->head = (q->head + 1) % q->nmemb;

	return 1;
}

