#include <UltraProto.h>
#include <Memory.h>
#include <RBTree.h>
#include <Macros.h>
#include <String.h>
#include <Types.h>
#include <Print.h>
#include "PMM.h"


static struct SMInfo sm_info = (struct SMInfo) { 0 };

static struct PMAllocator allocator = (struct PMAllocator) { 0 };


static void _RBPrint(struct RBNode *node)
{
	if(node == NULL) {
		Put("*");
		return;
	}

	struct PMBlock *nodeblk = PMBLOCK(node);

	if(node->left != NULL || node->right != NULL)
		Put("(");

	Put("0x%xi <=> 0x%xi", PMBLOCK_START(nodeblk), PMBLOCK_END(nodeblk));

	if(node->left != NULL) {
		Put(": ");

		_RBPrint(node->left);
	}

	if(node->right != NULL) {
		if(node->left == NULL)
			Put(": *");
		Put(", ");

		_RBPrint(node->right);
	}

	if(node->left != NULL || node->right != NULL)
		Put(")");
}

static void RBPrint()
{
	Put("        ");
	_RBPrint(allocator.tree.node);

	Put("\n");

	if(allocator.tree.leftest != NULL) {
		struct PMBlock *nodeblk = PMBLOCK(allocator.tree.leftest);
		Print(LOG_NONE, "Leftest:  0x%xi <=> 0x%xi\n", PMBLOCK_START(nodeblk), PMBLOCK_END(nodeblk));
		nodeblk = PMBLOCK(allocator.tree.rightest);
		Print(LOG_NONE, "Rightest: 0x%xi <=> 0x%xi\n", PMBLOCK_START(nodeblk), PMBLOCK_END(nodeblk));
	}
}

static void PMBlockInsert(struct PMBlock *blk)
{
	struct RBNode *parent = NULL;
	struct RBNode **node  = &allocator.tree.node;

	while(1) {
		if(*node == NULL)
			break;

		struct PMBlock *nodeblk = PMBLOCK(*node);

		parent = *node;

		uintptr_t bend = PMBLOCK_END(nodeblk);
		uintptr_t  end = PMBLOCK_END(blk);

		if(PMBLOCK_START(blk) < PMBLOCK_START(nodeblk) && end <= PMBLOCK_START(blk))
			node = &(*node)->left;
		else if(PMBLOCK_START(blk) >= bend && end >= bend)
			node = &(*node)->right;
		else
			Panic("PM blocks cant collide");
	}

	RBInsert(&allocator.tree, &blk->node, parent, node);

	if(blk->flags == PMFLAGS_NODE)
		allocator.rsvd_count++;

	allocator.buddy_count++;
}

static struct PMBlock *PMBlockFind(size_t start, size_t end)
{
	struct RBNode *node = allocator.tree.node;

	while(1) {
		if(node == NULL)
			return NULL;

		struct PMBlock *nodeblk = PMBLOCK(node);

		uintptr_t bend = PMBLOCK_END(nodeblk);

		if(start < PMBLOCK_START(nodeblk) && end <= PMBLOCK_START(nodeblk))
			node = node->left;
		else if(start >= bend && end > bend)
			node = node->right;
		else if(start == PMBLOCK_START(nodeblk) && end == bend)
			return nodeblk;
		else
			Panic("Tried to get unidentifiable range");
	}
}

static void PMBlockErase(struct PMBlock *blk)
{
	RBErase(&allocator.tree, &blk->node);
	allocator.buddy_count--;
}


void PMInit(struct ultra_memory_map_attribute *mmap)
{
	size_t count = MEMORY_MAP_ENTRY_COUNT(mmap->header);

	for(size_t i = 0; i < count; i++) {
		struct ultra_memory_map_entry *ent = &mmap->entries[i];

		if(ent->type != ULTRA_MEMORY_TYPE_FREE)
			continue;

		size_t ent_nodes = ent->size_in_bytes / 128;

		ent_nodes = ent_nodes < 4096 ? 4096 : ent_nodes;

		if((1ULL << 63) % ent_nodes != 0)
			ent_nodes = 1ULL << (log2(ent_nodes) + 1);

		Info("Ent nodes size: %u KiB, Entry size: %u KiB, Entry addr: 0x%xl\n", ent_nodes / 1024, ent->size_in_bytes / 1024, ent->physical_address);

		if(ent->size_in_bytes > ent_nodes) {
			struct PMBlock *block = phys_offset(ent->physical_address);

			*block = (struct PMBlock) {
				.start = ent->physical_address / 4096,
				.order = log2(ent_nodes / 4096),
				.flags = PMFLAGS_NODE
			};

			PMBlockInsert(block);

			Info("Created node block of order %u at 0x%xl\n", block->order, ent->physical_address);

			ent->size_in_bytes    -= ent_nodes;
			ent->physical_address += ent_nodes;
		}
	}

	RBPrint();
}

void SMInfoGet(struct SMInfo *info)
{
	*info = sm_info;
}


// Page allocation functions

uintptr_t PMAlloc(size_t npages)
{
	return npages;
}

void PMFree(uintptr_t addr)
{
	addr + 2;
	return;
}
