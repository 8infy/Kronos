#include <stddef.h>
#include <stdint.h>


void KernelInit()
{
	while(1)
		asm volatile("hlt");
}
