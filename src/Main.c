#include <UltraProto.h>
#include <stddef.h>
#include <stdint.h>
#include <Print.h>

void KernelInit(struct ultra_boot_context *boot_ctx, uint32_t magic)
{
	while(magic != ULTRA_MAGIC)
		asm volatile("hlt");


	


	while(1)
		asm volatile("hlt");
}
