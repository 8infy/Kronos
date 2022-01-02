#include <RBTree.h>


static INLINE void RBChildReplace(struct RBRoot *root, struct RBNode *node, struct RBNode *new)
{
	if(node == NULL) return;

	if(new != NULL)
		RBParentSet(new, RBParent(node));

	int leaf = RBLeaf(node);

	if(leaf == RB_LEFT) {
		RBParent(node)->left = new;
	} else if(leaf == RB_RIGHT) {
		RBParent(node)->right = new;
	} else {
		root->node = new;

		if(new != NULL)
			new->parent = 0;
	}
}

static void RBRotateLeft(struct RBRoot *root, struct RBNode *node)
{
	struct RBNode *parent = RBParent(node);
	struct RBNode *right  = node->right;

	node->right = right == NULL ? NULL : right->left;

	RBParentSet(node->right, node);

	RBParentSet(right, parent);

	if(RBLeaf(node) == RB_LEFT)
		parent->left = right;
	else if(RBLeaf(node) == RB_RIGHT)
		parent->right = right;
	else
		root->node = right;

	if(right != NULL)
		right->left = node;

	RBParentSet(node, right);
}

static void RBRotateRight(struct RBRoot *root, struct RBNode *node)
{
	struct RBNode *parent = RBParent(node);
	struct RBNode *left  = node->left;

	node->left = left == NULL ? NULL : left->right;

	RBParentSet(node->left, node);

	RBParentSet(left, parent);

	if(RBLeaf(node) == RB_LEFT)
		parent->left = left;
	else if(RBLeaf(node) == RB_RIGHT)
		parent->right = left;
	else
		root->node = left;

	if(left != NULL)
		left->right = node;

	RBParentSet(node, left);
}

static void RBFixHole(struct RBRoot *root, struct RBNode *parent, struct RBNode *node, int leaf)
{
	if(parent == NULL) {
		// Case 1

		RBColorSet(node, RB_BLACK);
		return;
	}

	struct RBNode *sibling = leaf == RB_LEFT ? parent->right : parent->left;

	struct RBNode *sleft  = sibling == NULL ? NULL : sibling->left;
	struct RBNode *sright = sibling == NULL ? NULL : sibling->right;

	if(RBColor(parent) == RB_BLACK && RBColor(sibling) == RB_RED &&
		RBColor(sleft) == RB_BLACK && RBColor(sright)  == RB_BLACK) {
		// Case 2

		RBColorSet(parent, RB_RED);
		RBColorSet(sibling, RB_BLACK);

		if(leaf == RB_LEFT)
			RBRotateLeft(root, parent);
		else
			RBRotateRight(root, parent);

		RBFixHole(root, parent, node, leaf);
		return;
	}

	if(RBColor(parent) == RB_BLACK && RBColor(sibling) == RB_BLACK &&
		RBColor(sleft) == RB_BLACK && RBColor(sright)  == RB_BLACK) {
		// Case 3

		RBColorSet(sibling, RB_RED);
		RBColorSet(node,    RB_BLACK);

		RBFixHole(root, RBParent(parent), parent, RBLeaf(parent));
		return;
	}

	if(RBColor(parent) == RB_RED   && RBColor(sibling) == RB_BLACK &&
		RBColor(sleft) == RB_BLACK && RBColor(sright)  == RB_BLACK) {
		// Case 4

		RBColorSet(parent,  RB_BLACK);
		RBColorSet(node,    RB_BLACK);
		RBColorSet(sibling, RB_RED);

		return;
	}

	if(leaf == RB_LEFT && RBColor(sibling) == RB_BLACK &&
		RBColor(sleft) == RB_RED && RBColor(sright) == RB_BLACK) {
		// Case 5L

		RBColorSet(sleft,   RB_BLACK);
		RBColorSet(sibling, RB_RED);

		RBRotateRight(root, sibling);

		RBFixHole(root, parent, node, leaf);
		return;
	}

	if(leaf == RB_RIGHT && RBColor(sibling) == RB_BLACK &&
		RBColor(sleft) == RB_BLACK && RBColor(sright) == RB_RED) {
		// Case 5R

		RBColorSet(sright,  RB_BLACK);
		RBColorSet(sibling, RB_RED);

		RBRotateLeft(root, sibling);

		RBFixHole(root, parent, node, leaf);
		return;
	}

	if(leaf == RB_LEFT && RBColor(sibling) == RB_BLACK && RBColor(sright) == RB_RED) {
		// Case 6L

		RBColorSet(sibling, RBColor(parent));
		RBColorSet(parent,  RB_BLACK);
		RBColorSet(sright,  RB_BLACK);
		RBColorSet(node,    RB_BLACK);

		RBRotateLeft(root, parent);
		return;
	}

	if(leaf == RB_RIGHT && RBColor(sibling) == RB_BLACK && RBColor(sleft) == RB_RED) {
		// Case 6R

		RBColorSet(sibling, RBColor(parent));
		RBColorSet(parent,  RB_BLACK);
		RBColorSet(sleft,   RB_BLACK);
		RBColorSet(node,    RB_BLACK);

		RBRotateRight(root, parent);
		return;
	}
}

void RBInsert(struct RBRoot *root, struct RBNode *node, struct RBNode *parent, struct RBNode **victim)
{
	if(*victim != NULL) {
		// We are replacing node with *victim

		node->parent = (*victim)->parent;

		node->left  = (*victim)->left;
		node->right = (*victim)->right;

		if(RBLeaf(*victim) == RB_LEFT)
			parent->left = node;
		if(RBLeaf(*victim) == RB_RIGHT)
			parent->right = node;

		return;
	}

	if(parent == NULL) {
		root->node   = node;
		node->parent = 0;
		node->left   = NULL;
		node->right  = NULL;

		return;
	}


	*victim = node;

	RBParentColorSet(node, parent, RB_RED);

	while(RBColor(parent) == RB_RED && RBColor(node) == RB_RED) {
		struct RBNode *aunt = RBSibling(parent);
		struct RBNode *g    = RBParent(parent);

		if(RBColor(aunt) == RB_RED) {
			RBColorSet(parent, RB_BLACK);
			RBColorSet(aunt,   RB_BLACK);

			if(g->parent == 0) return;

			RBColorSet(g, RB_RED);

			node   = g;
			parent = RBParent(node);
		} else {
			switch(RBLeaf(node) - RBLeaf(parent) * 2)
			{
			case 3: // LR
				RBRotateLeft(root, parent);
				parent = node;
			case 1: // LL
				RBRotateRight(root, g);
				break;
			case -3: // RL
				RBRotateRight(root, parent);
				parent = node;
			case -1: // RR
				RBRotateLeft(root, g);
				break;
			}

			RBColorSet(parent, RB_BLACK);
			RBColorSet(g,      RB_RED);
		}
	}

}

void RBErase(struct RBRoot *root, struct RBNode *node)
{
	struct RBNode *new    = NULL;
	struct RBNode *parent = RBParent(node);

	int lnode = RBLeaf(node);
	int cnode = RBColor(node);


	if(node->left == NULL || node->right == NULL) {
		new = node->left != NULL ? node->left : node->right;

		if(node->parent == 0) {
			root->node = new;

			if(new != NULL)
				new->parent = 0;

			return;
		}

		if(lnode == RB_LEFT)
			parent->left = new;
		else
			parent->right = new;

		RBParentSet(new, parent);

		if(RBColor(new) == RB_RED || cnode == RB_RED) {
			RBColorSet(new, RB_BLACK);
			return;
		}

		RBFixHole(root, parent, new, lnode);
	} else {
		new = node->right;

		while(new->left != NULL)
			new = new->left;

		int cnew            = RBColor(new);
		struct RBNode *pnew = RBParent(new);

		if(lnode == RB_LEFT)
			parent->left = new;
		else if(lnode == RB_RIGHT)
			parent->right = new;
		else
			root->node = new;

		new->parent = node->parent;
		new->left   = node->left;
		RBParentSet(node->left, new);


		if(pnew == node) {
			if(cnew == RB_BLACK) {
				if(new->right != NULL) {
					RBColorSet(new->right, RB_BLACK);
					return;
				}
				RBFixHole(root, new, new->right, RB_RIGHT);
			}
		} else {
			struct RBNode *nright = new->right;

			new->right = node->right;
			RBParentSet(node->right, new);

			pnew->left = nright;
			RBParentColorSet(nright, pnew, RB_BLACK);

			if(nright == NULL && cnew == RB_BLACK)
				RBFixHole(root, pnew, NULL, RB_LEFT);
		}
	}
}

