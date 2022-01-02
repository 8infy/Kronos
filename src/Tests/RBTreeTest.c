#include <RBTree.h>
#include <stdlib.h>
#include <stdio.h>


struct IntNode
{
	struct RBNode node;
	int          value;
};

#define INT_NODE(x) container_of(x, struct IntNode, node)


void RBPrint(struct RBNode *node)
{
	if(node == NULL) {
		putchar('B');
		return;
	}

	if(node->left == NULL && node->right == NULL) {
		printf("%d%c", INT_NODE(node)->value, RBColor(node) == RB_BLACK ? 'B' : 'R');
		return;
	} else {
		printf("(%d%c:", INT_NODE(node)->value, RBColor(node) == RB_BLACK ? 'B' : 'R');
	}

	RBPrint(node->left);
	if(node->right != NULL) {
		putchar(',');
		RBPrint(node->right);
	}
	putchar(')');
}

void RBCheckRed(struct RBNode *node)
{
	if(node == NULL)
		return;

	int nc = RBColor(node);
	if(nc == RB_BLACK) return;

	if(RBColor(node->left) == RB_RED) {
		printf("Red violation at left leaf of %d\n", INT_NODE(node)->value);
		exit(1);
	}
	if(RBColor(node->right) == RB_RED) {
		printf("Red violation at right leaf of %d\n", INT_NODE(node)->value);
		exit(1);
	}

	RBCheckRed(node->left);
	RBCheckRed(node->right);
}

int RBCheckBDepth(struct RBNode *node, int bdepth)
{
	if(node == NULL) return ++bdepth;

	if(RBColor(node) == RB_BLACK)
		++bdepth;

	int dl = RBCheckBDepth(node->left,  bdepth);
	int dr = RBCheckBDepth(node->right, bdepth);

	if(dl != dr) {
		printf("Black depth at %d's subtrees is not protected\n", INT_NODE(node)->value);
		exit(1);
	}

	return dl;
}

void RBCheckParent(struct RBNode *node)
{
	if(node->left != NULL) {
		if(RBParent(node->left) != node) {
			printf("Child parent is not parent at %d\n", INT_NODE(node)->value);
			exit(1);
		}

		RBCheckParent(node->left);
	}

	if(node->right != NULL) {
		if(RBParent(node->right) != node) {
			printf("Child parent is not parent at %d\n", INT_NODE(node)->value);
			exit(1);
		}

		RBCheckParent(node->right);
	}
}

void RBCheck(struct RBRoot *root)
{
	if(root->node == NULL) return;

	RBCheckParent(root->node);
	RBCheckRed(root->node);
	RBCheckBDepth(root->node, 0);
}

struct RBNode **MySearch(struct RBRoot *root, int value, struct RBNode **_parent)
{
	struct RBNode *parent = root->node;
	struct RBNode **node  = &root->node;

	if(parent == NULL) {
		*_parent = NULL;
		return node;
	}

	while(1) {
		if(value < INT_NODE(parent)->value)
			node = &parent->left;
		else if(value > INT_NODE(parent)->value)
			node = &parent->right;
		else
			break;

		if(*node == NULL) break;

		parent = *node;
	}

	*_parent = parent;
	return node;
}

void MyAdd(struct RBRoot *tree, int val)
{
	struct RBNode *parent, **node;

	node = MySearch(tree, val, &parent);

	if(*node != NULL) return;

	struct IntNode *int_node = calloc(1, sizeof(struct IntNode));
	int_node->value = val;

	RBInsert(tree, &int_node->node, parent, node);

	// RBPrint(tree->node);
	// putchar('\n');
	// RBCheck(tree);
}

void MyErase(struct RBRoot *tree, int val)
{
	struct RBNode *parent, **node;

	node = MySearch(tree, val, &parent);

	if(*node == NULL) return;

	RBErase(tree, *node);

	// RBPrint(tree->node);
	// putchar('\n');
	// RBCheck(tree);
}


int main()
{
	struct RBRoot my_tree = (struct RBRoot) { 0 };

	srand(time(NULL));

	int *fibb = calloc(200000, sizeof(int));

	int last = 1;

	for(int i = 0; i < 200000; i++) {
		fibb[i] = (rand() % 2000000) + 1;

		MyAdd(&my_tree, fibb[i]);
	}

	while(my_tree.node != NULL) {
		int r = rand() % 200000;

		if(fibb[r] == 0) continue;

		MyErase(&my_tree, fibb[r]);
		fibb[r] = 0;
	}

	RBCheck(&my_tree);

	return 0;
}
