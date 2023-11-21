#ifndef BTREE_H
#define BTREE_H

#include "config.h"
#include <stdlib.h>

struct BinaryTreeNode {
	btreeValue_t value;

	struct BinaryTreeNode *parent;
	struct BinaryTreeNode *left;
	struct BinaryTreeNode *right;
};

struct BinaryTree {
	struct BinaryTreeNode *root;
	size_t size;
};


int btreeCreate(struct BinaryTree *tree);
int btreeDelete(struct BinaryTree *tree);

struct BinaryTreeNode *btreeInsertNode(struct BinaryTree *tree,
		struct BinaryTreeNode *node, btreeValue_t value);
struct BinaryTreeNode *btreeDeleteNode(struct BinaryTree *tree,
		struct BinaryTreeNode *node);

void btreeDump(struct BinaryTree *tree, int level);

#endif // BTREE_H
