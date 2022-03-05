#include <UltraProto.h>
#include <Memory.h>
#include <Macros.h>
#include <String.h>
#include <Types.h>
#include <Print.h>
#include "PMM.h"


static struct SMInfo sm_info = (struct SMInfo) { 0 };

static struct PMAllocator pmm = (struct PMAllocator) { 0 };

static void PMPrint(struct RBRoot *tree);


static inline struct PMBlock *PMBlockNew()
{
	struct PMBlock *blk = NULL;

	if(pmm.blk_ind > 0) {
		blk = &pmm.blk[pmm.blk_count + pmm.blk_ind - 1];
		BUG_ON(!blk->ind);

		blk->ind = 0;
		pmm.blk_ind--;

		blk = &pmm.blk[blk->start];
	} else {
		blk = &pmm.blk[pmm.blk_count++];
	}

	*blk = (struct PMBlock) { 0 };
	return blk;
}

static inline void PMBlockIndirect(struct PMBlock *to_blk)
{
	BUG_ON(to_blk < pmm.blk)

	size_t index = to_blk - pmm.blk;

	struct PMBlock *blk = &pmm.blk[pmm.blk_count + pmm.blk_ind++];
	*blk = (struct PMBlock) { 0 };

	blk->ind   = 1;
	blk->start = index;
}

static void PMBlockInsertFree(struct PMBlock *blk)
{
	BUG_ON(blk == NULL);

	struct RBNode *parent = NULL;
	struct RBNode **node  = &pmm.free.node;

	while(*node != NULL) {
		struct PMBlock *nblk = PMBLOCK(*node);

		parent = *node;

		if(blk->start < nblk->start)
			node = &(*node)->left;
		else
			node = &(*node)->right;
	}

	RBInsert(&pmm.free, &blk->node, parent, node);
}

static void PMBlockInsertUsed(struct PMBlock *blk)
{
	BUG_ON(blk == NULL);

	struct RBNode *parent = NULL;
	struct RBNode **node  = &pmm.used.node;

	while(*node != NULL) {
		struct PMBlock *nblk = PMBLOCK(*node);

		parent = *node;

		if(blk->start < nblk->start)
			node = &(*node)->left;
		else
			node = &(*node)->right;
	}

	RBInsert(&pmm.used, &blk->node, parent, node);
}


static void PMBlockEraseFree(struct PMBlock *blk)
{
	RBErase(&pmm.free, &blk->node);
}

static void PMBlockEraseUsed(struct PMBlock *blk)
{
	RBErase(&pmm.used, &blk->node);
}


static int PMBlockBalanced(struct PMBlock *blk)
{
	struct RBNode *node = &blk->node;

	size_t left_size  = node->left  != NULL ? PMBLOCK(node->left)->size  : blk->size;
	size_t right_size = node->right != NULL ? PMBLOCK(node->right)->size : blk->size;
	struct PMBlock *parent = PMBLOCK(RBParent(node));

	int pchk = 0;

	if(parent != NULL)
		pchk = RBLeaf(node) == RB_LEFT ? blk->size > parent->size : blk->size < parent->size;

	return (blk->size < left_size) || (blk->size > right_size) || pchk;
}

static int PMAlignable(struct PMBlock *blk, size_t npages)
{
	BUG_ON(16777216 % npages != 0);

	size_t pad = npages - (blk->start & (npages - 1));
	return pad == npages ? 1 : (pad < blk->size && npages <= blk->size - pad);
}


static struct PMBlock *PMPageAlloc(size_t npages)
{
	npages = 1ULL << log2(npages);

	if(npages > 1024 || npages == 0)
		return NULL;

	struct RBNode *parent = NULL;
	struct RBNode *node   = pmm.free.leftest;
	struct PMBlock *nblk  = NULL;

	if(node == NULL)
		return NULL;

	while(node != NULL) {
		nblk   = PMBLOCK(node);
		parent = node;

		size_t fb = ((nblk->size + 1) / 2 + (nblk->size + 1) / 3);
		size_t fa = nblk->size + 1 - fb;
		if((npages >= fb || npages <= fa) && npages - 1 < nblk->size && PMAlignable(nblk, npages))
			break;

		if(npages - 1 == nblk->size && PMAlignable(nblk, npages))
			break;

		node = RBNext(node);
	}

	if(unlikely(node == NULL)) {
		do {
			node = parent;

			if(node == NULL)
				return NULL;

			parent = RBParent(node);
			nblk   = PMBLOCK(node);

			if(parent == NULL && (!PMAlignable(nblk, npages) || npages - 1 > nblk->size))
				return NULL;
		} while(npages - 1 > nblk->size || !PMAlignable(nblk, npages));
	}

	sm_info.phys_free -= FROM_PAGE(npages);
	sm_info.phys_used += FROM_PAGE(npages);

	struct PMBlock *align = NULL;
	if((nblk->start & (npages - 1)) != 0) {
		align = PMBlockNew();

		align->start = nblk->start;
		align->size  = npages - (nblk->start & (npages - 1)) - 1;
		nblk->start += align->size + 1;
		nblk->size  -= align->size + 1;
	}

	if(npages - 1 == nblk->size) {
		PMBlockEraseFree(nblk);
		PMBlockInsertUsed(nblk);

		if(align != NULL)
			PMBlockInsertFree(align);

		return nblk;
	}

	struct PMBlock *block = PMBlockNew();
	block->start = nblk->start;
	block->size  = npages - 1;
	PMBlockInsertUsed(block);

	nblk->start += npages;
	nblk->size  -= npages;

	if(!PMBlockBalanced(nblk)) {
		PMBlockEraseFree(nblk);
		PMBlockInsertFree(nblk);
	}

	if(align != NULL)
		PMBlockInsertFree(align);

	return block;
}

static void PMPatch()
{
	if(pmm.free.leftest == NULL) return;

	uint64_t end = 0;
	struct PMBlock *block = PMBLOCK(pmm.free.leftest);
	struct PMBlock *last  = NULL;

	size_t timeout = 0;

	while(block != NULL) {
		if(end == block->start && block->size + last->size + 1 < BLK_MAX_SIZE) {
			block->start = last->start;
			block->size += last->size + 1;

			last->ind = 1;
			last->start = 0;
			last->size = 0;
			PMBlockIndirect(last);
			PMBlockEraseFree(last);
			PMBlockEraseFree(block);
			PMBlockInsertFree(block);

			timeout = 0;
		}

		last = block;
		end  = block->start + block->size + 1;

		block = PMBLOCK(RBNext(&block->node));

		if(timeout++ > 3) break;
	}

}

static void PMPageFree(struct PMBlock *blk)
{
	sm_info.phys_free += FROM_PAGE(blk->size + 1);
	sm_info.phys_used -= FROM_PAGE(blk->size + 1);

	PMBlockEraseUsed(blk);
	PMBlockInsertFree(blk);

	PMPatch();
}


void SMInfoGet(struct SMInfo *info)
{
	SpinLock(&pmm.lock);
	*info = sm_info;
	SpinUnlock(&pmm.lock);
}


void PMInit(struct ultra_memory_map_attribute *mmap)
{
	size_t mmap_ent_count = ULTRA_MEMORY_MAP_ENTRY_COUNT(mmap->header);
	size_t total_free     = 0;
	size_t total_blocks   = 0;

	Info(    ".------------ System Memory Map ---------------.\n");
	Print(0, "|                                              |\n");
	Print(0, "|  Entry  Start     Entry    End    Entry Size |\n");
	/*        | 0000000000000000 0000000000000000 nnnnnn KiB |*/

	for(size_t i = 0; i < mmap_ent_count; i++) {
		struct ultra_memory_map_entry *ent = &mmap->entries[i];

		if(ent->type != ULTRA_MEMORY_TYPE_INVALID)
			sm_info.phys_total += ent->size;

		if(ent->type != ULTRA_MEMORY_TYPE_FREE) {
			sm_info.phys_rsvd += ent->size;
			continue;
		}

		total_free += ent->size;

		if(ent->physical_address >= 1048576)
			sm_info.phys_free += ent->size;

		uint64_t start = ent->physical_address;
		uint64_t end   = ent->physical_address + ent->size;
		uint64_t size  = ent->size;

		const char *unit = "B";
		unit = (size / 1048576 > 0) ? "MiB" : "KiB";

		size /= (size / 1048576 > 0) ? 1048576 : 1024;

		Print(0, "| %xl %xl %{ 6%u %s |\n", start, end, size, unit);
	}
	Print(0, "'----------------------------------------------'\n");

	total_blocks = FROM_PAGE(ceil(total_free / BLK_PER_PAGE, 4096));
	sm_info.phys_free -= total_blocks;
	sm_info.phys_rsvd += total_blocks;

	for(size_t i = 0; i < mmap_ent_count; i++) {
		struct ultra_memory_map_entry *ent = &mmap->entries[i];

		if(ent->type != ULTRA_MEMORY_TYPE_FREE)
			continue;

		if(ent->size > total_blocks) {
			pmm.blk       = phys_offset(ent->physical_address);
			pmm.blk_count = 0;
			pmm.blk_cap   = total_blocks / 32;

			ent->physical_address += total_blocks;
			ent->size    -= total_blocks;

			break;
		}
	}

	if(pmm.blk == NULL)
		Panic("Out of memory for the PMM");

	for(size_t i = 0; i < mmap_ent_count; i++) {
		struct ultra_memory_map_entry *ent = &mmap->entries[i];

		if(ent->type != ULTRA_MEMORY_TYPE_FREE || ent->physical_address < 1048576)
			continue;

		size_t size  = ent->size    / 4096;
		size_t start = ent->physical_address / 4096;

		while(size > 0) {
			struct PMBlock *blk = PMBlockNew();

			blk->start = start;
			blk->size  = min(size - 1, BLK_MAX_SIZE - 1);

			size  -= blk->size + 1;
			start += blk->size + 1;

			PMBlockInsertFree(blk);
		}
	}
}

// Page allocation functions

uintptr_t PMAlloc(size_t npages)
{
	SpinLock(&pmm.lock);

	struct PMBlock *blk = PMPageAlloc(npages);

	SpinUnlock(&pmm.lock);

	return blk == NULL ? 0 : FROM_PAGE(blk->start);
}

void PMFree(uintptr_t addr)
{
	SpinLock(&pmm.lock);

	struct RBNode *node     = pmm.used.node;
	struct PMBlock *nodeblk = NULL;

	addr /= 4096ULL;

	while(node != NULL) {
		nodeblk = PMBLOCK(node);

		if(addr < nodeblk->start)
			node = node->left;
		else if(addr > nodeblk->start)
			node = node->right;
		else
			break;
	}

	if(unlikely(node == NULL))
		Panic("Can't free page(s) at physical address %xl", FROM_PAGE(addr));

	PMPageFree(PMBLOCK(node));

	SpinUnlock(&pmm.lock);
}

static void PMPrint(struct RBRoot *tree)
{
	BUG_ON(tree != &pmm.free && tree != &pmm.used);

	const char *tree_name = tree == &pmm.free ? "Free  Memory Tree": "Used  Memory Tree";

	struct RBNode *node = tree->leftest;

	Info(    ".------------ %s ---------------.\n", tree_name);
	Print(0, "|                                              |\n");
	Print(0, "|  Entry  Start     Entry    End    Entry Size |\n");

	while(node != NULL) {
		struct PMBlock *blk = PMBLOCK(node);

		size_t size  = FROM_PAGE(blk->size + 1);
		size_t start = FROM_PAGE(blk->start);
		size_t end   = start + size;

		const char *unit = "B";
		unit = (size / 1048576 > 0) ? "MiB" : "KiB";
		size /= (size / 1048576 > 0) ? 1048576 : 1024;

		Print(0, "| %xl %xl %{ 6%u %s |\n", start, end, size, unit);

		node = RBNext(node);
	}

	Print(0, "'----------------------------------------------'\n");
}
