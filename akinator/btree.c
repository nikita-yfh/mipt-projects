#include "btree.h"
#include "log.h"
#include "hsv2rgb.h"
#include "tokener.h"
#include "utils.h"

#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

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
		struct BinaryTreeNode *node, const char *value) {
	assert(tree);

	if(!node ^ !tree->root)
		return NULL;

	if(node && node->left && node->right)
		return NULL;

	struct BinaryTreeNode *child = (struct BinaryTreeNode*)
			   calloc(1, sizeof(struct BinaryTreeNode) + strlen(value) + 1);
	
	if(!tree->root)
		tree->root = child;
	else if(!node->left)
		node->left = child;
	else if(!node->right)
		node->right = child;

	strcpy(child->value, value);
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
	}

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

	fprintf(dot, "Node%p[label=\"%s\" fillcolor=\"%s\" color=\"%s\"]\n;",
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

struct BinaryTreeNode *btreeFindLeaf(struct BinaryTreeNode *node, const char *name) {
	struct BinaryTreeNode *leftFinded = NULL, *rightFinded = NULL;
	if(node->left && (leftFinded = btreeFindLeaf(node->left, name)))
		return leftFinded;
	if(node->right && (rightFinded = btreeFindLeaf(node->right, name)))
		return rightFinded;
	if(strcmp(node->value, name) == 0)
		return node;
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

static int btreeWriteNode(const struct BinaryTreeNode *node, FILE *file) {
	fputc('(', file);
	fprintf(file, "\"%s\"", node->value);

	fputc(' ', file);

	if(node->left)
		btreeWriteNode(node->left, file);
	else
		fputs("nil", file);

	fputc(' ', file);

	if(node->right)
		btreeWriteNode(node->right, file);
	else
		fputs("nil", file);

	fputc(')', file);
	return 0;
}

static struct Token *btreeReadNode(struct BinaryTree *tree, struct BinaryTreeNode *parent, struct Token *token) {

#define CHECK_TOKEN(token)						\
	if(!tokenIsValid(token)) {		\
		printLog(LOG_ERROR, "EOF");				\
		return NULL;							\
	}

	CHECK_TOKEN(token);

	if(token->type == MODE_NAME && strcmp(token->text, "nil") == 0) {
		printLog(LOG_INFO, "Readed nil");
		return token + 1;
	}

	if(token->type != MODE_OPERATOR && strcmp(token->text, "(") != 0) {
		printLog(LOG_ERROR, "Invalid token %p \"%s\", need \"(\"", token, token->text);
		return NULL;
	}
	token++;
	CHECK_TOKEN(token);

	if(token->type != MODE_STRING) {
		printLog(LOG_ERROR, "Token type isn't string");
		return NULL;
	}
	
	struct BinaryTreeNode *node = btreeInsertNode(tree, parent, token->text);
	token++;

	printLog(LOG_INFO, "Readed value \"%s\"", node->value);

	for(int childIndex = 0; childIndex < 2; childIndex++) {
		token = btreeReadNode(tree, node, token);
		if(!token)
			return NULL;
	}

	CHECK_TOKEN(token);
	if(token->type != MODE_OPERATOR && strcmp(token->text, ")") != 0) {
		printLog(LOG_ERROR, "Invalid token \"%s\", need \")\"", token->text);
		return NULL;
	}
	token++;

	printLog(LOG_INFO, "Readed node %p with value \"%s\" with childs %p, %p",
			node, node->value, node->left, node->right);

	return token;

#undef CHECK_TOKEN
}

int btreeReadFile(struct BinaryTree *tree, FILE *file) {
	assert(tree);
	btreeDelete(tree);

	char *content = readFile(file);

	if(!content)
		return -1;

	struct Tokens tokens = tokensCreate(content);

	printLog(LOG_DEBUG, "Current tokens:");
	for(struct Token *token = tokens.tokens; tokenIsValid(token); token++)
		printLog(LOG_DEBUG, "Token [%s] %d", token->text, token->type);

	if(!btreeReadNode(tree, NULL, tokens.tokens))
		return -1;

	tokensDelete(&tokens);
	free(content);

	return 0;
}

int btreeWriteFile(const struct BinaryTree *tree, FILE *file) {
	btreeWriteNode(tree->root, file);
	return 0;
}
