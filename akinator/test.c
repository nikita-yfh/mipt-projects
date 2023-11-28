#include "btree.h"
#include "log.h"
#include "tokener.h"

int main() {
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

	btreeDump(&tree, LOG_DEBUG);

	btreeDeleteNode(&tree, n);
	btreeDump(&tree, LOG_DEBUG);

	FILE *file = fopen("out.txt", "w");
	btreeWriteFile(&tree, file);
	fclose(file);

	file = fopen("out.txt", "r");

	btreeReadFile(&tree, file);

	printLog(LOG_DEBUG, "After read:");
	btreeDump(&tree, LOG_DEBUG);

	fclose(file);

	btreeDelete(&tree);
	return 0;
}
