#include "list.h"
#include "log.h"

#include <stdlib.h>
#include <assert.h>

static void* myRealloc(void *mem, size_t num, size_t size) {
	if(mem)
		return realloc(mem, num * size);
	return calloc(num, size);
}

static int listResize(struct List *list) {
	list->nodes = (struct ListNode*)
		myRealloc(list->nodes, list->capacity, sizeof(struct ListNode*));

	return list->nodes != 0;
}

int listCreate(struct List *list) {
	list->capacity = LIST_INIT_CAPACITY;
	list->size = 0;

	listResize(list);

	return 0;
}

static int checkNodeIsNormal(struct List *list, struct ListNode *node) {
	return node->next >= 0 && node->next < list->capacity &&
	       node->prev >= 0 && node->prev < list->capacity;
}

struct ListNode *listGetNextNode(struct List *list, struct ListNode *node) {
	assert(checkNodeIsNormal(list, node));

	if(node->next <= 0)
		return NULL;
	return &list->nodes[node->next];
}

struct ListNode *listGetPrevNode(struct List *list, struct ListNode *node) {
	assert(checkNodeIsNormal(list, node));

	if(node->prev <= 0)
		return NULL;
	return &list->nodes[node->prev];
}
