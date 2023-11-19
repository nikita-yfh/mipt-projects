#include "list.h"
#include "log.h"

#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>

static void* myRealloc(void *mem, size_t num, size_t size) {
	if(mem)
		return realloc(mem, num * size);
	return calloc(num, size);
}

static int listResize(struct List *list, size_t freeIndex, size_t newCapacity) {
	assert(list);
	assert(freeIndex < newCapacity);

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
	assert(list);
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
	if(!list)
		return -1;

	free(list->values);
	free(list->prev);
	free(list->next);
	list->capacity = 0;

	printLog(LOG_INFO, "Destroyed list [%p]", list);

	return 0;
}

static int listCheckNode(const struct List *list, listIndex_t index) {
	assert(list);

	if(index >= list->capacity)
		return -1;
	if(list->next[index] >= list->capacity || list->prev[index] >= list->capacity)
		return -1;
	return 0;
}

static int listCheckUsableNode(const struct List *list, listIndex_t index) {
	assert(list);

	if(listCheckNode(list, index))
		return -1;
	if(list->prev[index] == LIST_INVALID_INDEX)
		return -1;
	if(list->next[index] == LIST_INVALID_INDEX)
		return -1;
	return 0;
}

static listIndex_t listUseNode(struct List *list, listIndex_t prev, listIndex_t next) {
	assert(list);

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

	list->next[prev] = freeIndex;
	list->prev[next] = freeIndex;
	return freeIndex;
}

static int listFreeNode(struct List *list, listIndex_t index) {
	assert(list);

	if(listCheckUsableNode(list, index))
		return -1;

	list->prev  [index] = LIST_INVALID_INDEX;
	list->next  [index] = list->freeNode;
	list->values[index] = LIST_DEFAULT_VALUE;

	list->freeNode = index;

	return 0;
}

listIndex_t listGetNextNode(const struct List *list, listIndex_t index) {
	assert(list);

	if(listCheckUsableNode(list, index))
		return LIST_INVALID_INDEX;
	return list->next[index];
}

listIndex_t listGetPrevNode(const struct List *list, listIndex_t index) {
	assert(list);

	if(listCheckUsableNode(list, index))
		return LIST_INVALID_INDEX;
	return list->prev[index];
}

int listDeleteNode(struct List *list, listIndex_t index) {
	assert(list);

	if(listCheckUsableNode(list, index))
		return -1;

	listIndex_t prev = list->prev[index];
	listIndex_t next = list->next[index];

	list->next[prev] = next;
	list->prev[next] = prev;

	listFreeNode(list, index);
	return 0;
}

listIndex_t listInsertNodeBefore(struct List *list, listIndex_t next, listValue_t value) {
	assert(list);

	if(listCheckUsableNode(list, next))
		return LIST_INVALID_INDEX;

	listIndex_t newIndex = listUseNode(list, list->prev[next], next);
	list->values[newIndex] = value;
	return newIndex;
}

listIndex_t listInsertNodeAfter(struct List *list, listIndex_t prev, listValue_t value) {
	assert(list);

	if(listCheckUsableNode(list, prev))
		return LIST_INVALID_INDEX;

	listIndex_t newIndex = listUseNode(list, prev, list->next[prev]);
	list->values[newIndex] = value;
	return newIndex;
}

listIndex_t listGetHead(const struct List *list) {
	assert(list);

	return list->next[LIST_DUMMY_INDEX];
}

listIndex_t listGetTail(const struct List *list) {
	assert(list);

	return list->prev[LIST_DUMMY_INDEX];
}

listValue_t *listGetValue(struct List *list, listIndex_t index) {
	assert(list);
	assert(!listCheckUsableNode(list, index));

	return &list->values[index];
}

const listValue_t *listGetValueC(const struct List *list, listIndex_t index) {
	assert(list);
	assert(!listCheckUsableNode(list, index));

	return &list->values[index];
}

int listDump(const struct List *list, int level) {
	assert(list);

	char tmpFileName[128] = "";

	snprintf(tmpFileName, sizeof(tmpFileName), P_tmpdir"/graph%d.dot", getpid());

	FILE *dot = fopen(tmpFileName, "w");

	fprintf(dot, "digraph G {\n");
	fprintf(dot, "splines=ortho;\n");
	fprintf(dot, "node [shape=box style=filled];\n");
	fprintf(dot, "{ rank = same; HEAD; TAIL; FREE; }\n");

	fprintf(dot, "HEAD->Node%lu;\n", listGetHead(list));
	fprintf(dot, "TAIL->Node%lu;\n", listGetTail(list));
	fprintf(dot, "FREE->Node%lu;\n", list->freeNode);

	for(listIndex_t index = LIST_DUMMY_INDEX; index < list->capacity; index++) {
		const char *color = "";
		if(index != LIST_DUMMY_INDEX)
			color = "color=\"#00FF00\" fillcolor=\"#AAFFAA\"";
		if(list->prev[index] == LIST_INVALID_INDEX)
			color = "color=\"#FF0000\" fillcolor=\"#FFAAAA\"";

		fprintf(dot, "Node%lu[%s shape=record "
				"label=\"{Node%lu|"LIST_FORMAT"|prev = %ld|next = %ld}\"];\n",
			  	index, color, index, list->values[index],
				(long) list->prev[index], (long) list->next[index]);

		if(list->next[index] < list->capacity)
			fprintf(dot, "Node%lu->Node%lu;\n", index, list->next[index]);
		if(list->prev[index] < list->capacity)
			fprintf(dot, "Node%lu->Node%lu;\n", index, list->prev[index]);
	}

	fprintf(dot, "{ rank = same; ");
	for(listIndex_t index = LIST_DUMMY_INDEX; index < list->capacity; index++)
		fprintf(dot, "Node%lu ", index);
	fprintf(dot, "}\n");

	fprintf(dot, "}\n");

	fclose(dot);

	insertGraphLog(level, tmpFileName, "List [%p] dump:", list);
	remove(tmpFileName);

	return 0;
}


