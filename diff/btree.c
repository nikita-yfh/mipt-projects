#include "btree.h"
#include "log.h"
#include "hsv2rgb.h"
#include "utils.h"
#include "operations.h"

#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

void btreeDelete(struct BinaryTreeNode *node) {
	if(!node)
		return;
	btreeDelete(node->left);
	btreeDelete(node->right);

	free(node);
}

static struct BinaryTreeNode *btreeNew() {
	struct BinaryTreeNode *node = (struct BinaryTreeNode*)
		calloc(1, sizeof(struct BinaryTreeNode));
	return node;
}

struct BinaryTreeNode *btreeNewConst(double value) {
	struct BinaryTreeNode *node = btreeNew();
	node->type = NODE_CONST;
	node->value = value;
	return node;
}

struct BinaryTreeNode *btreeNewOperator(operator_t operator) {
	struct BinaryTreeNode *node = btreeNew();
	node->type = NODE_OPERATION;
	node->operator = operator;
	return node;
}

static void btreeDumpNode(FILE *dot, unsigned int level, const struct BinaryTreeNode *node) {
	assert(dot);

	if(!node)
		return;

	char fillColor[8];
	char edgeColor[8];

	double hue = level * 40.0;
	struct HSV fill = {hue, 0.3, 1.0};
	struct HSV edge = {hue, 1.0, 1.0};

	char value[100] = "";

	if(node->type == NODE_CONST)
		snprintf(value, sizeof(value), "%g", node->value);
	else if(node->type == NODE_OPERATION) {
		const struct Operator *operator = operatorGet(node->operator);
		if(operator)
			strcpy(value, operator->name);
	}

	fprintf(dot, "Node%p[label=\"%s\" fillcolor=\"%s\" color=\"%s\"]\n;",
		node, value, HSVToRGB(fillColor, fill), HSVToRGB(edgeColor, edge));
	if(node->parent)
		fprintf(dot, "Node%p->Node%p;\n", node->parent, node);

	if(node->left)
		btreeDumpNode(dot, level + 1, node->left);
	if(node->right)
		btreeDumpNode(dot, level + 1, node->right);
}

void btreeDump(const struct BinaryTreeNode *tree, int level) {
	assert(tree);

	char tmpFileName[128] = "";
	snprintf(tmpFileName, sizeof(tmpFileName), P_tmpdir"/graph%d.dot", getpid());

	FILE *dot = fopen(tmpFileName, "w");

	fprintf(dot, "digraph G {\n");
	fprintf(dot, "node[shape=box style=filled];\n");
	btreeDumpNode(dot, 0, tree);
	fprintf(dot, "}\n");

	fclose(dot);

	insertGraphLog(level, tmpFileName, "Binary tree %p dump:", tree);
	remove(tmpFileName);
}

