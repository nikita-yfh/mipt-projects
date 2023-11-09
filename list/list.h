#ifndef LIST_H
#define LIST_H

#include "config.h"
#include <stdlib.h>

struct ListNode {
	listValue_t value;
	listIndex_t prev;
	listIndex_t next;
};

struct List {
	size_t capacity;
	size_t size;

	struct ListNode *nodes;

	listIndex_t headLink;
	listIndex_t tailLink;
	listIndex_t freeLink;
};

int listCreate(struct List *list);
int listDelete(struct List *list);

struct ListNode *listGetHead(struct List *list);
struct ListNode *listGetTail(struct List *list);

struct ListNode *listInsertNodeBefore(struct List *list, struct ListNode *node, listValue_t value);
struct ListNode *listInsertNodeAfter (struct List *list, struct ListNode *node, listValue_t value);

struct ListNode *listGetNextNode(struct List *list, struct ListNode *node);
struct ListNode *listGetPrevNode(struct List *list, struct ListNode *node);

struct ListNode *listDeleteNode(struct List *list, struct ListNode *node);

int listVerify(const struct List *list);
int listDump(const struct List *list);

#endif

