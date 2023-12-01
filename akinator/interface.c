#include "btree.h"
#include "log.h"
#include "colors.h"
#include "stack.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>

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

static void objectNotFound(const char *name) {
	printf("Объект с названием \"%s\" не найден\n", name);
}

static void showInfo(struct BinaryTree *tree, const char *name) {
	struct BinaryTreeNode *finded = btreeFindLeaf(tree->root, name);
	if(!finded) {
		objectNotFound(name);
		return;
	}
	printf("Свойства \"%s\":\n", name);
	int index = 1;
	while(finded != tree->root) {
		printf("%d) %s%s\n", index++, (finded->parent->left == finded) ? "не " : "", finded->parent->value);
		finded = finded->parent;
	}
}

static void compareObjects(struct BinaryTree *tree, const char *objectA, const char *objectB) {
	struct BinaryTreeNode *nodeA = btreeFindLeaf(tree->root, objectA);
	struct BinaryTreeNode *nodeB = btreeFindLeaf(tree->root, objectB);

	if(!nodeA)
		objectNotFound(objectA);
	if(!nodeB)
		objectNotFound(objectB);

	struct Stack stackA, stackB;
	btreeFillStack(nodeA, &stackA);
	btreeFillStack(nodeB, &stackB);

	if(nodeA && nodeB) {
		printf(COLOR_MAGENTA"Сравнение \"%s\" и \"%s\":\n"COLOR_NONE, objectA, objectB);

		int dirA = 0, dirB = 0;
		struct BinaryTreeNode *node = tree->root;

		printf("Сходства: "COLOR_GREEN);
		while(!stackPop(&stackA, &dirA) && !stackPop(&stackB, &dirB) && dirA == dirB) {
			printf("%s%s, ", (dirA == NODE_LEFT) ? "не " : "", node->value);
			node = btreeGetChild(node, dirA);
		}
		fputs(COLOR_NONE"все!\n", stdout);

		nodeA = nodeB = node;
		printf("Особенности \"%s\": "COLOR_RED, objectA);
		do {
			printf("%s%s, ", (dirA == NODE_LEFT) ? "не " : "", nodeA->value);
			nodeA = btreeGetChild(nodeA, dirA);
		} while(!stackPop(&stackA, &dirA));
		fputs(COLOR_NONE"все!\n", stdout);

		printf("Особенности \"%s\": "COLOR_RED, objectB);
		do {
			printf("%s%s, ", (dirB == NODE_LEFT) ? "не " : "", nodeB->value);
			nodeB = btreeGetChild(nodeB, dirB);
		} while(!stackPop(&stackB, &dirB));
		fputs(COLOR_NONE"все!\n", stdout);

		fputc('\n', stdout);
	}

	stackDelete(&stackA);
	stackDelete(&stackB);
}

static void invalidInput() {
	fputs(COLOR_RED"Неверный ввод. Повторите попытку: "COLOR_NONE, stdout);
}

static int isYesNo(const char *c) {
	assert(c);
	if(*c == 'y' || *c == 'Y' || strncmp("Д", c, 2) == 0 || strncmp("д", c, 2) == 0)
		return 1;
	if(*c == 'n' || *c == 'N' || strncmp("Н", c, 2) == 0 || strncmp("н", c, 2) == 0)
		return 0;
	return -1;
}

static int askYesNo(const char *prompt) {
	fputs(COLOR_CYAN,	stdout);
	fputs(prompt,		stdout);
	fputs(" (y/n): ",	stdout);
	fputs(COLOR_NONE,	stdout);

	char answer[30];

	do {
		fputs(COLOR_GREEN,	stdout);
		fgets(answer, sizeof(answer), stdin);
		fputs(COLOR_NONE,	stdout);

		int result = isYesNo(answer);
		if(result >= 0)
			return result;

		invalidInput();
	} while(1);
}

static int askNaturalNumber(const char *prompt, int maxNumber) {
	fputs(COLOR_CYAN,	stdout);
	fputs(prompt,		stdout);
	printf(" (1-%d): ", maxNumber);
	fputs(COLOR_NONE,	stdout);

	int result = 0;
	do {
		fputs(COLOR_GREEN,	stdout);

		if(scanf("%d", &result) == 1 && result >= 1 && result <= maxNumber)
			break;

		while(getchar() != '\n');

		invalidInput();
	} while(1);

	fputs(COLOR_NONE,	stdout);
	while(getchar() != '\n');

	return result;
}

static void askString(const char *prompt, char *str, int num) {
	fputs(COLOR_CYAN,	stdout);
	fputs(prompt,		stdout);
	fputs(" ",			stdout);
	fputs(COLOR_NONE,	stdout);

	fputs(COLOR_GREEN,	stdout);
	fgets(str, num,		stdin);
	fputs(COLOR_NONE,	stdout);

	*strchr(str, '\n') = '\0';
}

enum {
	MENU_ASK,
	MENU_COMPARE,
	MENU_DESCRIPTION,
	MENU_EXIT,

	MENU_COUNT
};

static void showMenu(struct BinaryTree *tree) {
	int choice = 0;
	while(choice - 1 != MENU_EXIT) {
		fputs(
			COLOR_MAGENTA
			"Выбери действие:\n"
			COLOR_NONE
			"1) Отгадать объект\n"
			"2) Сравнить объекты\n"
			"3) Вывести определение объекта\n"
			"4) Выход\n", stdout);

		choice = askNaturalNumber("Введи номер пункта", MENU_COUNT);

		switch(choice - 1) {
		case MENU_COMPARE: {
			char object1[100], object2[100];
			askString("Введи имя объекта №1:", object1, sizeof(object1));
			askString("Введи имя объекта №2:", object2, sizeof(object2));
			compareObjects(tree, object1, object2);
		} break;
		case MENU_DESCRIPTION: {
			char object[100];
			askString("Введи имя объекта:", object, sizeof(object));
			showInfo(tree, object);
		} break;
		}
	}
}

int main() {
	struct BinaryTree tree = {};
	readTree(&tree, "tree.txt");

	btreeDump(&tree, LOG_DEBUG);

	showMenu(&tree);

	if(askYesNo("Сохранить дерево?"))
		writeTree(&tree, "tree.txt");

	return 0;
}
