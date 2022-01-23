#pragma once

#include <stddef.h>
#include <stdint.h>


#define PACKED __attribute__((packed))

#define ALWAYS_INLINE __attribute__((always_inline)) inline

#define container_of(ptr, T, member) ((T *) ((void *) ptr - offsetof(T, member)))

#define is_constexpr(x) __builtin_constant_p(x)


#define min(a, b) is_constexpr(a) && is_constexpr(b) ? ((a) < (b) ? (a) : (b)) :   \
                  ({ typeof(a) ___ta = a, ___tb = b; ___ta < ___tb ? ___ta : ___tb; })


#define max(a, b) is_constexpr(a) && is_constexpr(b) ? ((a) > (b) ? (a) : (b)) :   \
                  ({ typeof(a) ___ta = a, ___tb = b; ___ta > ___tb ? ___ta : ___tb; })
