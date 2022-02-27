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
