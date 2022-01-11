#pragma once

#include <stddef.h>
#include <stdint.h>


#define INLINE __attribute__((always_inline)) inline

#define container_of(ptr, T, member) ((T *) ((void *) ptr - offsetof(T, member)))

static INLINE size_t min(size_t a, size_t b)
{
	return a > b ? b : a;
}

static INLINE size_t max(size_t a, size_t b)
{
	return a > b ? a : b;
}
