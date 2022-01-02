#pragma once

#include <stddef.h>
#include <stdint.h>


#define INLINE __attribute__((always_inline)) inline

#define container_of(ptr, T, member) ((T *) ((void *) ptr - offsetof(T, member)))
