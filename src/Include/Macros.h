#pragma once

#include <stddef.h>
#include <stdint.h>


#define PAGE_SIZE 4096
#define PHYS_BASE 0xFFFF800000000000
#define KRNL_BASE 0xFFFFFFFF80000000


#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)


#define PACKED __attribute__((packed))

#define ALWAYS_INLINE __attribute__((always_inline)) inline

#define container_of(ptr, T, member) ((T *) ((void *) ptr - offsetof(T, member)))

#define is_constexpr(x) __builtin_constant_p(x)


#define min(a, b) (is_constexpr(a) && is_constexpr(b) ? ((a) < (b) ? (a) : (b)) :     \
                  ({ typeof(a) ___ta = a, ___tb = b; ___ta < ___tb ? ___ta : ___tb; }))

#define max(a, b) (is_constexpr(a) && is_constexpr(b) ? ((a) > (b) ? (a) : (b)) :     \
                  ({ typeof(a) ___ta = a, ___tb = b; ___ta > ___tb ? ___ta : ___tb; }))


#define log2(x) (x == 0 ? -1 : 63 - __builtin_clzll(x))

#define hang() do { asm volatile("cli"); while(1) asm volatile("hlt"); } while(0)
