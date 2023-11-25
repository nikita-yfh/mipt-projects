#include "btree.h"
#include "log.h"
#include "tokener.h"

int main() {
	const char *str = "int main +(5+4)var()";
	struct Tokens tokens = tokensCreate(str);

	tokensDelete(&tokens);

	struct BinaryTree tree = {};
	btreeCreate(&tree);


	printLog(LOG_DEBUG, "test");

	btreeInsertNode(&tree, NULL, "test");
	struct BinaryTreeNode *l = btreeInsertNode(&tree, tree.root, "a");
	struct BinaryTreeNode *n = btreeInsertNode(&tree, tree.root, "b");
	btreeInsertNode(&tree, n, "d");
	btreeInsertNode(&tree, n, "e");
	btreeInsertNode(&tree, l, "f");
	struct BinaryTreeNode *g = btreeInsertNode(&tree, l, "g");
	btreeInsertNode(&tree, g, "h");
	for(int i = 0; i < 20; i++) {
		g = btreeInsertNode(&tree, g, "tt");
		btreeInsertNode(&tree, g, "h");
	}

	btreeDump(&tree, LOG_DEBUG);

	btreeDeleteNode(&tree, n);
	btreeDump(&tree, LOG_DEBUG);


	FILE *file = fopen("out.txt", "w");
	btreeWriteFile(&tree, file);
	fclose(file);

	btreeDelete(&tree);
	return 0;
}
