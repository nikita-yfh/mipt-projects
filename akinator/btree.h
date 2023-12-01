#ifndef BTREE_H
#define BTREE_H

#include <stdlib.h>
#include <stdio.h>

#include "stack.h"

enum {
	NODE_LEFT,
	NODE_RIGHT
};

struct BinaryTreeNode {
	struct BinaryTreeNode *parent;
	struct BinaryTreeNode *left;
	struct BinaryTreeNode *right;

	char value[0];
};

struct BinaryTree {
	struct BinaryTreeNode *root;
	size_t size;
};

int btreeCreate(struct BinaryTree *tree);
int btreeDelete(struct BinaryTree *tree);

struct BinaryTreeNode *btreeInsertNode(struct BinaryTree *tree,
		struct BinaryTreeNode *node, const char *value);
struct BinaryTreeNode *btreeDeleteNode(struct BinaryTree *tree,
		struct BinaryTreeNode *node);

struct BinaryTreeNode *btreeFindLeaf     (struct BinaryTreeNode *node,
		const char *name);

void btreeFillStack(struct BinaryTreeNode *node, struct Stack *stack);

struct BinaryTreeNode *btreeGetChild(struct BinaryTreeNode *node, int lr);

void btreeDump(const struct BinaryTree *tree, int level);

int btreeWriteFile(const struct BinaryTree *tree, FILE *file);
int  btreeReadFile(      struct BinaryTree *tree, FILE *file);

#endif // BTREE_H

