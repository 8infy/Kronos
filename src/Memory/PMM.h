#pragma once

#include <Memory.h>
#include <RBTree.h>
#include <Macros.h>
#include <Types.h>


#define PMBLOCK(rbnode) container_of(rbnode, struct PMBlock, node)

#define PMBLOCK_END(blk) (blk->start * 4096 + (1ULL << blk->order) * 4096)

#define PMBLOCK_START(blk) (blk->start * 4096)


#define PMFLAGS_FREE 0
#define PMFLAGS_USED 1
#define PMFLAGS_NODE 2

struct PMBlock
{
	struct RBNode node;
	uint64_t     start : 32;
	uint64_t     order :  6;
	uint64_t     flags :  2;
	uint64_t      rsvd : 24;
};

ASSERT_SIZE(struct PMBlock, 32);

struct PMAllocator
{
	struct RBRoot     tree;
	size_t     buddy_count;
	size_t     alloc_count;
	size_t      rsvd_count;
};

