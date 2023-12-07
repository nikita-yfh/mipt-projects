#include "btree.h"
#include "log.h"
#include "hsv2rgb.h"
#include "utils.h"

#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int btreeCreate(struct BinaryTree *tree) {
	assert(tree);

	tree->root = NULL; tree->size = 0;
	return 0;
}

int btreeDelete(struct BinaryTree *tree) {
	if(!tree)
		return -1;
	btreeDeleteNode(tree, tree->root);
	tree->root = NULL;

	assert(tree->size == 0);
	return 0;
}

struct BinaryTreeNode *btreeInsertNode(struct BinaryTree *tree,
		struct BinaryTreeNode *node) {
	assert(tree);

	if(!node ^ !tree->root)
		return NULL;

	if(node && node->left && node->right)
		return NULL;

	struct BinaryTreeNode *child = (struct BinaryTreeNode*)
			   calloc(1, sizeof(struct BinaryTreeNode));
	
	if(!tree->root)
		tree->root = child;
	else if(!node->left)
		node->left = child;
	else if(!node->right)
		node->right = child;

	child->parent = node;

	tree->size++;
	return child;
}

struct BinaryTreeNode *btreeDeleteNode(struct BinaryTree *tree,
		struct BinaryTreeNode *node) {
	assert(tree);
	if(!node)
		return NULL;

	printLog(LOG_VERBOSE, "Delete node %p with value %s", node, node->value);

	if(node->left)
		btreeDeleteNode(tree, node->left);
	if(node->right)
		btreeDeleteNode(tree, node->right);

	struct BinaryTreeNode *parent = node->parent;

	if(parent) {
		if(parent->left == node)
			parent->left = NULL;
		else if(parent->right == node)
			parent->right = NULL;
	} else
		tree->root = NULL;

	assert(tree->size != 0);
	tree->size--;

	free(node);

	return parent;
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

	if(node->type == NODE_NUMBER)
		snprintf(value, sizeof(value), "%g", node->value);
	else if(node->type == NODE_VARIABLE) {
		value[0] = node->variable;
		value[1] = '\0';
	} else if(node->type == NODE_OPERATION) {
		const char *name = getOperationName(node->operation);
		if(name)
			strcpy(value, name);
	}

	fprintf(dot, "Node%p[label=\"%s\" fillcolor=\"%s\" color=\"%s\"]\n;",
		node, value, HSVToRGB(fillColor, fill), HSVToRGB(edgeColor, edge));
	if(node->parent)
		fprintf(dot, "Node%p->Node%p;\n", node->parent, node);
	else
		fprintf(dot, "Tree->Node%p;\n", node);

	if(node->left)
		btreeDumpNode(dot, level + 1, node->left);
	if(node->right)
		btreeDumpNode(dot, level + 1, node->right);
}

struct BinaryTreeNode *btreeGetChild(struct BinaryTreeNode *node, int lr) {
	if(lr == NODE_LEFT  && node->left)
		return node->left;
	if(lr == NODE_RIGHT && node->right)
		return node->right;
	return NULL;
}

void btreeDump(const struct BinaryTree *tree, int level) {
	assert(tree);

	char tmpFileName[128] = "";
	snprintf(tmpFileName, sizeof(tmpFileName), P_tmpdir"/graph%d.dot", getpid());

	FILE *dot = fopen(tmpFileName, "w");

	fprintf(dot, "digraph G {\n");
	fprintf(dot, "node[shape=box style=filled];\n");
	fprintf(dot, "Tree[shape=record label=\"{Binary tree|size=%zu}\"]\n", tree->size);
	btreeDumpNode(dot, 0, tree->root);
	fprintf(dot, "}\n");

	fclose(dot);

	insertGraphLog(level, tmpFileName, "Binary tree %p dump:", tree);
}
