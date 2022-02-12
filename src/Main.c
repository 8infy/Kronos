#include <UltraProto.h>
#include <Types.h>
#include <Print.h>


void KernelInit(struct ultra_boot_context *boot_ctx, uint32_t magic)
{
	while(magic != ULTRA_MAGIC)
		asm volatile("hlt");

	struct ultra_attribute_header *h = boot_ctx->attributes;
	uint32_t i = boot_ctx->attribute_count;

	while(--i && h->type != ULTRA_ATTRIBUTE_KERNEL_INFO)
		h = NEXT_ATTRIBUTE(h);

	Info("Kernel physical base: %xl\n", ((struct ultra_kernel_info_attribute *) h)->physical_base);
	Info("Kernel virtual  base: %xl\n", ((struct ultra_kernel_info_attribute *) h)->virtual_base);
	Info("Kernel total    size: %uB\n", ((struct ultra_kernel_info_attribute *) h)->range_length);

	

	while(1)
		asm volatile("hlt");
}
