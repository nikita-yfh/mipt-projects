#ifndef BTREE_H
#define BTREE_H

#include <stdlib.h>
#include <stdio.h>

#include "operations.h"

enum {
	NODE_LEFT,
	NODE_RIGHT
};

enum NodeType {
	NODE_NUMBER,
	NODE_VARIABLE,
	NODE_OPERATION,
};

struct BinaryTreeNode {
	struct BinaryTreeNode *parent;
	struct BinaryTreeNode *left;
	struct BinaryTreeNode *right;

	enum NodeType type;

	union {
		double value;
		char variable;
		operation_t operation; 
	};
};

struct BinaryTree {
	struct BinaryTreeNode *root;
	size_t size;
};

int btreeCreate(struct BinaryTree *tree);
int btreeDelete(struct BinaryTree *tree);

struct BinaryTreeNode *btreeInsertNode(struct BinaryTree *tree, struct BinaryTreeNode *node);
struct BinaryTreeNode *btreeDeleteNode(struct BinaryTree *tree, struct BinaryTreeNode *node);

struct BinaryTreeNode *btreeFindLeaf     (struct BinaryTreeNode *node,
		const char *name);

struct BinaryTreeNode *btreeGetChild(struct BinaryTreeNode *node, int lr);

void btreeDump(const struct BinaryTree *tree, int level);

#endif // BTREE_H

