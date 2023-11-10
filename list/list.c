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
	list->values = (listValue_t*) myRealloc(list->nodes, list->capacity, sizeof(listValue_t));
	list->prev   = (listIndex_t*) myRealloc(list->nodes, list->capacity, sizeof(listIndex_t));
	list->next   = (listIndex_t*) myRealloc(list->nodes, list->capacity, sizeof(listIndex_t));

	for(listIndex_t index = list->freeIndex; index < list->capacity; index++) {
		list->next[index]  = index + 1;
		list->prev[index]  = LIST_INVALID_INDEX;
		list->value[index] = LIST_DEFAULT_VALUE;
	}

	return list->nodes != NULL;
}

int listCreate(struct List *list) {
	list->capacity = LIST_INIT_CAPACITY;

	list->headNode = list->tailNode = 0;
	list->freeNode = 1;

	if(listResize(list))
		return -1;
}

int listDelete(struct List *list) {
	free(list->values);
	free(list->prev);
	free(list->next);
	list->capacity = 0;
	return 0;
}

static struct ListNode *listGetNormalNode(struct List *list, listIndex_t index) {
	assert(index < list->capacity);

	struct ListNode *node = &list->nodes[index];
	assert(node->next < list->capacity && node->prev < list->capacity);

	return node;
}

static index_t listUseNode(struct List *list, index_t prev, index_t next) {
	if(list->freeNode >= list->capacity) {
		assert(list->freeNode == list->capacity);
		list->capacity = list->capacity * LIST_RESIZE_KOEF;
		listResize(list);
	}
	index_t freeIndex = list->freeNode;
	list->freeNode = node->next;
	list->prev[freeIndex] = prev;
	list->next[freeIndex] = next;
	return freeIndex;
}

static void listFreeNode(struct List *list, listIndex_t index) {
	struct ListNode *node = listGetNormalNode(list, index);

	node->prev = LIST_INVALID_INDEX;
	node->next = list->freeNode;

	list->freeNode = index;
}

listIndex_t listGetNextNode(struct List *list, listIndex_t index) {
	struct ListNode *node = listGetNormalNode(list, index);
	return node->next;
}

listIndex_t listGetPrevNode(struct List *list, listIndex_t index) {
	struct ListNode *node = listGetNormalNode(list, index);
	return node->prev;
}

listIndex_t listDeleteNode(struct List *list, listIndex_t index) {
	struct ListNode *node = listGetNormalNode(list, index);

	listIndex_t prev = node->prev, next = node->next;
	listGetNormalNode(list, prev)->next = next;
	listGetNormalNode(list, next)->prev = prev;

	listFreeNode(list, index);
	return prev;
}

