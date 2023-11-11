#include "list.h"
#include "log.h"

#include <stdlib.h>
#include <assert.h>

static void* myRealloc(void *mem, size_t num, size_t size) {
	if(mem)
		return realloc(mem, num * size);
	return calloc(num, size);
}

static int listResize(struct List *list, size_t freeIndex, size_t newCapacity) {
	list->values = (listValue_t*) myRealloc(list->values, newCapacity, sizeof(listValue_t));
	list->prev   = (listIndex_t*) myRealloc(list->prev,   newCapacity, sizeof(listIndex_t));
	list->next   = (listIndex_t*) myRealloc(list->next,   newCapacity, sizeof(listIndex_t));

	if(!list->values || !list->prev || !list->next) {
		printLog(LOG_ERROR, "Failed to resize list to %zu", newCapacity);
		return -1;
	}

	for(listIndex_t index = freeIndex; index < newCapacity; index++) {
		list->next[index]   = index + 1;
		list->prev[index]   = LIST_INVALID_INDEX;
		list->values[index] = LIST_DEFAULT_VALUE;
	}
	list->freeNode = freeIndex;
	list->capacity  = newCapacity;

	return 0;
}

int listCreate(struct List *list) {
	static_assert(LIST_INIT_CAPACITY != 0);

	list->values = NULL;
	list->prev = list->next = NULL;

	if(listResize(list, 1, LIST_INIT_CAPACITY))
		return -1;

	list->next[LIST_DUMMY_INDEX]   = LIST_DUMMY_INDEX;
	list->prev[LIST_DUMMY_INDEX]   = LIST_DUMMY_INDEX;
	list->values[LIST_DUMMY_INDEX] = LIST_DEFAULT_VALUE;

	printLog(LOG_INFO, "Created list [%p] of capacity %zu", list, LIST_INIT_CAPACITY);

	return 0;
}

int listDelete(struct List *list) {
	free(list->values);
	free(list->prev);
	free(list->next);
	list->capacity = 0;

	printLog(LOG_INFO, "Destroyed list [%p]", list);

	return 0;
}

static void listCheckNode(const struct List *list, listIndex_t index) {
	assert(index < list->capacity);
	assert(list->next[index] < list->capacity && list->prev[index] < list->capacity);
}

static void listCheckUsableNode(const struct List *list, listIndex_t index) {
	listCheckNode(list, index);
	assert(list->prev[index] != LIST_INVALID_INDEX);
	assert(list->next[index] != LIST_INVALID_INDEX);
}

static listIndex_t listUseNode(struct List *list, listIndex_t prev, listIndex_t next) {
	if(list->freeNode >= list->capacity) {
		size_t newCapacity = list->capacity * LIST_RESIZE_KOEF;
		printLog(LOG_INFO, "Need to resize list from %zu to %zu", list->capacity, newCapacity);
		assert(list->freeNode == list->capacity);
		listResize(list, list->freeNode, newCapacity);
	}
	listIndex_t freeIndex = list->freeNode;
	list->freeNode = list->next[freeIndex];
	list->prev[freeIndex] = prev;
	list->next[freeIndex] = next;
	return freeIndex;
}

static void listFreeNode(struct List *list, listIndex_t index) {
	listCheckUsableNode(list, index);

	list->prev[index] = LIST_INVALID_INDEX;
	list->next[index] = list->freeNode;

	list->freeNode = index;
}

listIndex_t listGetNextNode(const struct List *list, listIndex_t index) {
	listCheckUsableNode(list, index);
	return list->next[index];
}

listIndex_t listGetPrevNode(const struct List *list, listIndex_t index) {
	listCheckUsableNode(list, index);
	return list->prev[index];
}

listIndex_t listDeleteNode(struct List *list, listIndex_t index) {
	listCheckUsableNode(list, index);

	listIndex_t prev = list->prev[index];
	listIndex_t next = list->next[index];

	list->next[prev] = next;
	list->prev[next] = prev;

	listFreeNode(list, index);
	return prev;
}

listIndex_t listInsertNodeBefore(struct List *list, listIndex_t next, listValue_t value) {
	listCheckUsableNode(list, next);
	listIndex_t newIndex = listUseNode(list, list->prev[next], next);
	list->values[newIndex] = value;
	return newIndex;
}

listIndex_t listInsertNodeAfter(struct List *list, listIndex_t prev, listValue_t value) {
	listCheckUsableNode(list, prev);
	listIndex_t newIndex = listUseNode(list, prev, list->next[prev]);
	list->values[newIndex] = value;
	return newIndex;
}

listIndex_t listGetHead(const struct List *list) {
	return list->next[LIST_DUMMY_INDEX];
}

listIndex_t listGetTail(const struct List *list) {
	return list->prev[LIST_DUMMY_INDEX];
}

