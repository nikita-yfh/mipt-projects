#include "btree.h"
#include "log.h"

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

	if(node->left)
		btreeDeleteNode(tree, node->left);
	if(node->right)
		btreeDeleteNode(tree, node->right);

	struct BinaryTreeNode *parent = node->parent;
	free(node);

	return parent;
}

static void btreeDumpNode(FILE *dot, struct BinaryTreeNode *node) {
	assert(dot);

	if(!node)
		return;

	fprintf(dot, "Node%p[label=\""BTREE_FORMAT"\"]\n;", node, node->value);
	if(node->parent)
		fprintf(dot, "Node%p->Node%p;\n", node->parent, node);
	else
		fprintf(dot, "Tree->Node%p;\n", node);

	if(node->left)
		btreeDumpNode(dot, node->left);
	if(node->right)
		btreeDumpNode(dot, node->right);
}

void btreeDump(struct BinaryTree *tree, int level) {
	assert(tree);

	char tmpFileName[128] = "";
	snprintf(tmpFileName, sizeof(tmpFileName), P_tmpdir"/graph%d.dot", getpid());

	FILE *dot = fopen(tmpFileName, "w");

	fprintf(dot, "digraph G {\n");
	fprintf(dot, "node[shape=box style=filled];\n");
	fprintf(dot, "Tree[shape=record label=\"{Binary tree|size=%zu}\"]\n", tree->size);
	btreeDumpNode(dot, tree->root);
	fprintf(dot, "}\n");

	fclose(dot);

	insertGraphLog(level, tmpFileName, "Binary tree %p dump:", tree);
}

