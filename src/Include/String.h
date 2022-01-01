#pragma once

#include <stddef.h>
#include <stdint.h>


void *memcpy(void *dest, const void *src, size_t n);

void *memmove(void *dest, const void *src, size_t n);

void *memset(void *dest, int c, size_t n);

void *memmem(const void *haystack, const void *needle, size_t hlen, size_t nlen);

int memcmp(const void *s1, const void *s2, size_t n);


size_t strnlen(const char *s, size_t len);
