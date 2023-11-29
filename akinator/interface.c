#include "btree.h"
#include "log.h"

static int readTree(struct BinaryTree *tree, const char *fileName) {
	FILE *file = fopen(fileName, "r");
	if(!file || btreeReadFile(tree, file)) {
		printLog(LOG_ERROR, "Failed to read file \"%s\"", fileName);

		btreeCreate(tree);
		btreeInsertNode(tree, NULL, "Неизвестно кто");

		printLog(LOG_WARNING, "Created new empty tree");
	}
	else
		printLog(LOG_INFO, "Tree was readed from file \"%s\"", fileName);
	if(file)
		fclose(file);
	return 0;
}

static int writeTree(struct BinaryTree *tree, const char *fileName) {
	FILE *file = fopen(fileName, "w");
	if(!file) {
		printLog(LOG_ERROR, "Failed to open file \"%s\"", fileName);
		return -1;
	}
	if(btreeWriteFile(tree, file)) {
		printLog(LOG_ERROR, "Failed to write tree to file \"%s\"", fileName);
		return -1;
	}
	printLog(LOG_INFO, "Tree was written to file \"%s\"", fileName);
	return 0;
}

static void showInfo(struct BinaryTree *tree, const char *name) {
	struct BinaryTreeNode *finded = btreeFindLeaf(tree->root, name);
	if(!finded) {
		printf("Объект с названием \"%s\" не найден\n", name);
		return;
	}
	printf("Свойства \"%s\":\n", name);
	int index = 1;
	while(finded != tree->root) {
		const char *ne = "";
		if(finded->parent->left == finded)
			ne = "не ";
		printf("%d) %s%s\n", index++, ne, finded->value);

		finded = finded->parent;
	}
}

int main() {
	struct BinaryTree tree = {};
	readTree(&tree, "tree.txt");

	btreeDump(&tree, LOG_INFO);
	showInfo(&tree, "ппп");

	writeTree(&tree, "tree.txt");
	return 0;
}
