#include "btree.h"
#include "log.h"

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
	for(int i = 0; i < 20; i++) {
		g = btreeInsertNode(&tree, g, "tt");
		btreeInsertNode(&tree, g, "h");
	}

	btreeDump(&tree, LOG_DEBUG);

	btreeDeleteNode(&tree, n);
	btreeDump(&tree, LOG_DEBUG);

	btreeDelete(&tree);
	return 0;
}
