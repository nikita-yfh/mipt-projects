#include "btree.h"
#include "log.h"
#include "hsv2rgb.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>

int btreeCreate(struct BinaryTree *tree) {
	assert(tree);

	tree->root = NULL;
	tree->size = 0;
	return 0;
}

int btreeDelete(struct BinaryTree *tree) {
	if(!tree)
		return -1;
	btreeDeleteNode(tree, tree->root);
	tree->root = NULL;
	return (tree->size == 0) ? 0 : -1;
}

struct BinaryTreeNode *btreeInsertNode(struct BinaryTree *tree,
		struct BinaryTreeNode *node, btreeValue_t value) {
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

	child->value = value;
	child->parent = node;
	return child;
}

struct BinaryTreeNode *btreeDeleteNode(struct BinaryTree *tree,
		struct BinaryTreeNode *node) {
	assert(tree);
	if(!node)
		return NULL;

	printLog(LOG_VERBOSE, "Delete node %p with value "BTREE_FORMAT, node, node->value);

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
	}

	free(node);

	return parent;
}

static void btreeDumpNode(FILE *dot, unsigned int level, struct BinaryTreeNode *node) {
	assert(dot);

	if(!node)
		return;

	char fillColor[8];
	char edgeColor[8];

	double hue = level * 20.0;
	struct HSV fill = {hue, 0.3, 1.0};
	struct HSV edge = {hue, 1.0, 1.0};

	fprintf(dot, "Node%p[label=\""BTREE_FORMAT"\" fillcolor=\"%s\" color=\"%s\"]\n;",
		node, node->value, HSVToRGB(fillColor, fill), HSVToRGB(edgeColor, edge));
	if(node->parent)
		fprintf(dot, "Node%p->Node%p;\n", node->parent, node);
	else
		fprintf(dot, "Tree->Node%p;\n", node);

	if(node->left)
		btreeDumpNode(dot, level + 1, node->left);
	if(node->right)
		btreeDumpNode(dot, level + 1, node->right);
}

void btreeDump(struct BinaryTree *tree, int level) {
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

