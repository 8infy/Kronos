#include <stddef.h>
#include <stdint.h>
#include <Print.h>

void KernelInit()
{
	Info("Hello, world!\n");

	while(1)
		asm volatile("hlt");
}
