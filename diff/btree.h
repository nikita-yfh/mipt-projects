#ifndef BTREE_H
#define BTREE_H

#include <stdlib.h>
#include <stdio.h>

#include "lexer.h"

enum NodeType {
	NODE_CONST,
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
		unsigned int variable;
		operator_t operator; 
	};
};

struct BinaryTreeNode *btreeNewConst(double value);
struct BinaryTreeNode *btreeNewOperation(operator_t operator);

void btreeDelete(struct BinaryTreeNode *node);

void btreeDump(const struct BinaryTreeNode *tree, int level);

#endif // BTREE_H

