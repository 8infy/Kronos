#pragma once

#include <Types.h>

#define KERNEL_CS 0x08ULL
#define KERNEL_DS 0x10ULL
#define USER_CS   0x1BULL
#define USER_DS   0x23ULL


struct Processor
{
};


void GDTLoad();

void IDTLoad();


static inline uint64_t FlagsGet()
{
	uint64_t flags = 0;
	asm volatile("pushf\npop %0" : "=g"(flags));
	return flags;
}
