#ifndef BTREE_H
#define BTREE_H

#include <stdlib.h>
#include <stdio.h>

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

struct BinaryTreeNode *btreeGetChild(struct BinaryTreeNode *node, int lr);

void btreeDump(const struct BinaryTree *tree, int level);

#endif // BTREE_H

