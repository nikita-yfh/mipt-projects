#include "btree.h"
#include "log.h"
#include "colors.h"
#include "stack.h"
#include "speaker.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

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
	printspeakf("Объект с названием \"%s\" не найден\n", name);
}

static void showInfo(struct BinaryTree *tree, const char *name) {
	struct BinaryTreeNode *finded = btreeFindLeaf(tree->root, name);
	if(!finded) {
		objectNotFound(name);
		return;
	}
	printspeakf("Свойства \"%s\":\n", name);
	int index = 1;
	while(finded != tree->root) {
		printspeakf("%d) %s%s\n", index++, (finded->parent->left == finded) ? "не " : "", finded->parent->value);
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
		fputs(COLOR_MAGENTA, stdout);
		printspeakf("Сравнение \"%s\" и \"%s\":\n", objectA, objectB);
		fputs(COLOR_NONE, stdout);

		int dirA = 0, dirB = 0;
		struct BinaryTreeNode *node = tree->root;

		printspeakf("Сходства: ");
		fputs(COLOR_GREEN, stdout);

		while(!stackPop(&stackA, &dirA) && !stackPop(&stackB, &dirB) && dirA == dirB) {
			printspeakf("%s%s, ", (dirA == NODE_LEFT) ? "не " : "", node->value);
			node = btreeGetChild(node, dirA);
		}

		fputs(COLOR_NONE, stdout);
		printspeakf("все!\n");

		nodeA = nodeB = node;
		printspeakf("Особенности \"%s\": ", objectA);
		fputs(COLOR_RED, stdout);

		do {
			printspeakf("%s%s, ", (dirA == NODE_LEFT) ? "не " : "", nodeA->value);
			nodeA = btreeGetChild(nodeA, dirA);
		} while(!stackPop(&stackA, &dirA));

		fputs(COLOR_NONE, stdout);
		printspeakf("все!\n");

		printspeakf("Особенности \"%s\": ", objectB);
		fputs(COLOR_RED, stdout);

		do {
			printspeakf("%s%s, ", (dirB == NODE_LEFT) ? "не " : "", nodeB->value);
			nodeB = btreeGetChild(nodeB, dirB);
		} while(!stackPop(&stackB, &dirB));

		fputs(COLOR_NONE, stdout);
		printspeakf("все!\n");

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
	fflush(stdout);

	speakf(prompt);

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
	fflush(stdout);

	speakf(prompt);

	fputs(COLOR_GREEN,	stdout);
	fgets(str, num,		stdin);
	fputs(COLOR_NONE,	stdout);

	*strchr(str, '\n') = '\0';
}

static void saveNewObject(struct BinaryTree *tree, struct BinaryTreeNode *node) {
	char oldObjectStr[1024];
	strncpy(oldObjectStr, node->value, sizeof(oldObjectStr));

	char newObjectStr[1024];
	askString("А что же это было?", newObjectStr, sizeof(newObjectStr));

	char questionStr[3000];
	snprintf(questionStr, sizeof(questionStr), "Чем отличается \"%s\" от \"%s\"?",
			newObjectStr, oldObjectStr);

	char diffStr[1024];
	askString(questionStr, diffStr, sizeof(diffStr));

	struct BinaryTreeNode *lastNode = btreeDeleteNode(tree, node);
	btreeDump(tree, LOG_DEBUG);
	struct BinaryTreeNode *diffNode = btreeInsertNode(tree, lastNode, diffStr);
	btreeDump(tree, LOG_DEBUG);
	btreeInsertNode(tree, diffNode, oldObjectStr);
	btreeDump(tree, LOG_DEBUG);
	btreeInsertNode(tree, diffNode, newObjectStr);
	btreeDump(tree, LOG_DEBUG);
}

static void askObject(struct BinaryTree *tree) {
	btreeDump(tree, LOG_DEBUG);

	struct BinaryTreeNode *node = tree->root, *lastNode = tree->root;
	int dir = NODE_LEFT;
	while(node) {
		char question[1024] = "Это ";
		snprintf(question, sizeof(question), "Это %s?", node->value);
		dir = askYesNo(question);

		lastNode = node;
		node = btreeGetChild(node, dir);
	}
	if(dir == NODE_RIGHT) {
		printspeakf("Отгадал получается!\n");
		return;
	}
	saveNewObject(tree, lastNode);
	printspeakf("Запомнил!\n");

	btreeDump(tree, LOG_DEBUG);
}

enum {
	MENU_ASK,
	MENU_COMPARE,
	MENU_DESCRIPTION,
	MENU_RTRTRT,
	MENU_EXIT,

	MENU_COUNT
};

static void printRT() {
	srand((unsigned int) time(NULL));
	const char *strings[] = {
		"эРТэ лучший факультет физтеха",
		"от коробки до энка хуй сосет фээртэка",
		"зачем фивту подруга, когда есть жопа друга",
		"для любого эпсилона больше нуля фупм хуйня",
		"один паяльник, один мудак. два паяльника, два мудака."
			"три паяльника, три мудака. четыре паяльника, фээртэка",
		"слышен крик на всю долгопу, как фивты ебутся в жопу"
	};
	int number = (int) rand() % (int) (sizeof(strings) / sizeof(*strings));
	speakf(strings[number]);
}

static void showMenu(struct BinaryTree *tree) {
	fputs(COLOR_YELLOW, stdout);
	printspeakf(
			"Привет, я Акинатор-бета. Я могу отгадать тот объект, что записан в моем\n"
			"бинарном дереве, про существование других объектов пока не знаю. Поэтому\n"
			"если я что то не угадаю и тебя это устраивать не будет, то ты пойдешь нахуй\n"
			"и должен будешь объяснить, что же ты загадал.\n");
	fputs(COLOR_NONE, stdout);

	int choice = 0;
	while(choice - 1 != MENU_EXIT) {
		printf(
			COLOR_MAGENTA
			"Выбери действие:\n"
			COLOR_NONE
			"1) Отгадать объект\n"
			"2) Сравнить объекты\n"
			"3) Вывести определение объекта\n"
			"4) РТ РТ РТ РТ РТ\n"
			"5) Выход\n");

		speakf("Выбери действие");

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
		case MENU_ASK:
			askObject(tree);
			break;
		case MENU_RTRTRT:
			printRT();
			break;
		}
	}
}

int main() {
	struct BinaryTree tree = {};
	readTree(&tree, "tree.txt");

	showMenu(&tree);

	if(askYesNo("Сохранить дерево?"))
		writeTree(&tree, "tree.txt");

	return 0;
}
