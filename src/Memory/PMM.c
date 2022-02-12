#include <Memory.h>
#include <RBTree.h>
#include <Macros.h>
#include <String.h>
#include <Types.h>


enum PMFlags
{
	PMFLAGS_FREE,
	PMFLAGS_USED,
	PMFLAGS_RSVD
};

struct PMBlock
{
	struct RBNode node;
	uintptr_t    start;
	uintptr_t      end;
	enum PMFlags flags;
};

struct PMAllocator
{
	struct RBRoot     tree;
	size_t     buddy_count;
	size_t     alloc_count;
	size_t      rsvd_count;
};


static struct SMInfo sm_info = (struct SMInfo) { 0 };


void SMInfoGet(struct SMInfo *info)
{
	*info = sm_info;
}


// Page allocation functions

uintptr_t PMAlloc(size_t npages)
{
	return 0;
}

void PMFree(uintptr_t addr)
{
}
