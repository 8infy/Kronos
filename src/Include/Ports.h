#pragma once

#include <Macros.h>
#include <Types.h>


static inline void Out8(uint16_t port, uint8_t data)
{
	asm volatile("outb %0, %1" :: "a"(data), "Nd"(port));
}

static inline void Out16(uint16_t port, uint16_t data)
{
	asm volatile("outw %0, %1" :: "a"(data), "Nd"(port));
}

static inline void Out32(uint16_t port, uint32_t data)
{
	asm volatile("outl %0, %1" :: "a"(data), "Nd"(port));
}


static inline uint8_t In8(uint16_t port)
{
	uint8_t r = 0;
	asm volatile("inb %1, %0" : "=a"(r) : "Nd"(port));
	return r;
}

static inline uint16_t In16(uint16_t port)
{
	uint16_t r = 0;
	asm volatile("inw %1, %0" : "=a"(r) : "Nd"(port));
	return r;
}

static inline uint32_t In32(uint16_t port)
{
	uint32_t r = 0;
	asm volatile("inl %1, %0" : "=a"(r) : "Nd"(port));
	return r;
}
