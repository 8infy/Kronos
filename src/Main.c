#include <stddef.h>
#include <stdint.h>
#include <Print.h>

void KernelInit()
{

	while(1)
		asm volatile("hlt");
}
