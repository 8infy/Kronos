#include <UltraProto.h>
#include <Macros.h>
#include <Memory.h>
#include <Types.h>
#include <Print.h>


void PMInit(struct ultra_memory_map_attribute *mmap);


void KernelInit(struct ultra_boot_context *boot_ctx, uint32_t magic)
{
	LogInit();

	while(magic != ULTRA_MAGIC)
		Panic("Bootloader magic is not correct");

	struct ultra_attribute_header *h = boot_ctx->attributes;

	struct ultra_kernel_info_attribute *kinfo = NULL;
	struct ultra_memory_map_attribute *mmap   = NULL;

	int32_t i = boot_ctx->attribute_count;

	while(--i >= 0) {
		if(h->type == ULTRA_ATTRIBUTE_KERNEL_INFO)
			kinfo = (struct ultra_kernel_info_attribute *) h;
		if(h->type == ULTRA_ATTRIBUTE_MEMORY_MAP)
			mmap = (struct ultra_memory_map_attribute *) h;

		h = NEXT_ATTRIBUTE(h);
	}

	Info("Kernel physical base: %xl\n", kinfo->physical_base);
	Info("Kernel virtual  base: %xl\n", kinfo->virtual_base);
	Info("Kernel total    size: %uB\n", kinfo->range_length);

	PMInit(mmap);

	struct SMInfo info;
	SMInfoGet(&info);

	Info("SMInfo:    Total memory: %u MiB\n", info.phys_total / 1048576);
	Info("SMInfo:     Free memory: %u MiB\n", info.phys_free  / 1048576);
	Info("SMInfo:     Used memory: %u KiB\n", info.phys_used  / 1024);
	Info("SMInfo: Reserved memory: %u MiB\n", info.phys_rsvd  / 1048576);

	hang();
}
