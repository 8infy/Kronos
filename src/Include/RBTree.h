#pragma once

#include <stddef.h>
#include <stdint.h>
#include <Macros.h>


#define RB_BLACK 0
#define RB_RED   1

#define RB_LEFT  -1
#define RB_RIGHT  1


struct RBNode
{
	uintptr_t     parent;
	struct RBNode  *left;
	struct RBNode *right;
} __attribute__((packed, aligned(8)));

struct RBRoot
{
	struct RBNode *node;
};


void RBInsert(struct RBRoot *root, struct RBNode *node, struct RBNode *parent, struct RBNode **victim);

void RBErase(struct RBRoot *root, struct RBNode *node);


static inline int RBColor(struct RBNode *node)
{
	return node == NULL ? RB_BLACK : (node->parent & 1);
}

static inline struct RBNode *RBParent(struct RBNode *node)
{
	return node == NULL ? NULL : ((struct RBNode *) (node->parent & ~1));
}

static inline int RBLeaf(struct RBNode *node)
{
	struct RBNode *parent = RBParent(node);
	return parent == NULL ? 0 : (parent->left == node ? RB_LEFT : RB_RIGHT);
}

static inline struct RBNode *RBSibling(struct RBNode *node)
{
	struct RBNode *parent = RBParent(node);
	return parent == NULL ? NULL : (RBLeaf(node) == RB_LEFT ? parent->right : parent->left);
}

static inline void RBParentSet(struct RBNode *node, struct RBNode *parent)
{
	if(node == NULL) return;
	node->parent = (node->parent & 1) | (uintptr_t) parent;
}

static inline void RBParentColorSet(struct RBNode *node, struct RBNode *parent, int c)
{
	if(node == NULL) return;
	node->parent = parent == NULL ? 0 : (c | (uintptr_t) parent);
}

static inline void RBColorSet(struct RBNode *node, int color)
{
	if(node == NULL) return;
	node->parent = (node->parent & ~1) | color;
}


static inline struct RBNode *RBLeftest(struct RBNode *node)
{
	if(node == NULL) return NULL;

	while(node->left != NULL)
		node = node->left;

	return node;
}

static inline struct RBNode *RBRightest(struct RBNode *node)
{
	if(node == NULL) return NULL;

	while(node->right != NULL)
		node = node->right;

	return node;
}

static inline struct RBNode *RBNext(struct RBNode *node)
{
	if(node == NULL) return NULL;

	if(node->right != NULL) {
		node = node->right;

		while(node->left != NULL)
			node = node->left;

		return node;
	}

	if(RBLeaf(node) == RB_LEFT)
		return RBParent(node);

	do {
		node = RBParent(node);
	} while(RBLeaf(node) == RB_RIGHT);

	return RBParent(node);
}

static inline struct RBNode *RBPrev(struct RBNode *node)
{
	if(node == NULL) return NULL;

	if(node->left != NULL) {
		node = node->left;

		while(node->right != NULL)
			node = node->right;

		return node;
	}

	if(RBLeaf(node) == RB_RIGHT)
		return RBParent(node);

	do {
		node = RBParent(node);
	} while(RBLeaf(node) == RB_LEFT);

	return RBParent(node);
}
