#ifndef LIST_H
#define LIST_H

#include <stdint.h>
#include <stdlib.h>
#include "config.h"

struct List {
	size_t capacity;

	listValue_t *values;
	listIndex_t *prev;
	listIndex_t *next;

	listIndex_t headNode;
	listIndex_t tailNode;
	listIndex_t freeNode;
};

int listCreate(struct List *list);
int listDelete(struct List *list);

listIndex_t listGetHead(struct List *list);
listIndex_t listGetTail(struct List *list);

listIndex_t listInsertNodeBefore(struct List *list, listIndex_t node, listValue_t value);
listIndex_t listInsertNodeAfter (struct List *list, listIndex_t node, listValue_t value);

listIndex_t listGetNextNode(struct List *list, listIndex_t node);
listIndex_t listGetPrevNode(struct List *list, listIndex_t node);

listIndex_t listDeleteNode(struct List *list, listIndex_t node);

int listVerify(const struct List *list);
int listDump(const struct List *list);

#endif

