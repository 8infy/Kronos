#pragma once

#include <Memory.h>
#include <RBTree.h>
#include <Macros.h>
#include <Types.h>
#include <Lock.h>


#define PMBLOCK(rbnode) container_of(rbnode, struct PMBlock, node)

#define FROM_PAGE(x) ((x) * 4096ULL)

#define BLK_PER_PAGE (4096ULL / sizeof(struct PMBlock))

#define BLK_MAX_SIZE 4194304ULL


struct PMBlock
{
	struct RBNode node;
	uint64_t     start : 36; // (physical_addr / 4096)
	uint64_t      size : 22; // (size_in_bytes / 4096 - 1)
	uint64_t       ind :  1; // If this bit is set, start is an index to another PMBlock
	uint64_t      rsvd :  5; // Reserved
} __attribute__((packed, aligned(8)));

ASSERT_SIZE(struct PMBlock, 32);

struct PMAllocator
{
	struct RBRoot  free; // Free node tree
	struct RBRoot  used; // Used node tree

	struct PMBlock *blk; // First block
	size_t    blk_count; // Block count
	size_t      blk_ind; // Indirect block count
	size_t      blk_cap; // Block capacity
	spinlock_t     lock; // Allocator lock
};
