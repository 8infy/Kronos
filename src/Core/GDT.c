#include <Macros.h>
#include <Types.h>
#include <Core.h>

struct SegmentDesc
{
	uint64_t  limit : 16;
	uint64_t   base : 24;
	uint64_t access :  8;
	uint64_t limit2 :  4;
	uint64_t  flags :  4;
	uint64_t  base2 :  8;
} PACKED;

struct GDTR
{
	uint16_t size;
	uint64_t addr;
} PACKED;


static struct SegmentDesc descriptors[7] = {
	{ 0 },                                                            // NULL entry
	{ .limit = 0xFFFF, .limit2 = 0xF, .access = 0x9A, .flags = 0xA }, // Kernel code
	{ .limit = 0xFFFF, .limit2 = 0xF, .access = 0x92, .flags = 0x4 }, // Kernel data
	{ .limit = 0xFFFF, .limit2 = 0xF, .access = 0xFA, .flags = 0xA }, // Userspace code
	{ .limit = 0xFFFF, .limit2 = 0xF, .access = 0xF2, .flags = 0x4 }, // Userspace data
	{ 0 },                                                            // TSS
	{ 0 }                                                             // TSS
};

static struct GDTR descriptor_ptr = (struct GDTR) {
	.addr = (uint64_t) descriptors,
	.size = 55
};


void GDTLoad()
{
	// Set TSS here

	asm volatile("lgdt %0\n"
		"pushq %%rbp\n"
		"movq %%rsp, %%rbp\n"
		"pushq %1\n"
		"pushq %%rbp\n"
		"pushfq\n"
		"pushq %2\n"
		"pushq $1f\n"
		"iretq\n"
		"1:\n"
		"popq %%rbp\n"
		"mov %1, %%ds\n"
		"mov %1, %%es\n"
		"mov %1, %%fs\n"
		"mov %1, %%gs\n"
		"mov %1, %%ss\n"
		:: "m"(descriptor_ptr), "r"(KERNEL_DS), "r"(KERNEL_CS)
		: "memory");
}
