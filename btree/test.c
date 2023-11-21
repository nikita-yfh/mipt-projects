#include "btree.h"
#include "log.h"

int main() {
	struct BinaryTree tree = {};
	btreeCreate(&tree);

	printLog(LOG_DEBUG, "test");

	btreeInsertNode(&tree, NULL, 42);
	struct BinaryTreeNode *l = btreeInsertNode(&tree, tree.root, 22);
	struct BinaryTreeNode *n = btreeInsertNode(&tree, tree.root, 32);
	btreeInsertNode(&tree, n, 67);
	btreeInsertNode(&tree, n, 39);
	btreeInsertNode(&tree, l, 67);
	btreeInsertNode(&tree, l, 39);

	btreeDump(&tree, LOG_DEBUG);

	btreeDelete(&tree);
	return 0;
}
