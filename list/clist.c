#include "clist.h"
#include "log.h"

#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>

struct CListNode *clistInsertAfter(struct CListNode *prev, clistValue_t value) {
	struct CListNode *newNode = (struct CListNode*) calloc(1, sizeof(struct CListNode));

	if(newNode) {
		newNode->prev = prev;
		if(prev) {
			newNode->next = prev->next;
			if(newNode->next)
				newNode->next->prev = newNode;
			prev->next = newNode;
		} else
			newNode->prev = NULL;
		newNode->value = value;
	}

	return newNode;
}

struct CListNode *clistInsertBefore(struct CListNode *next, clistValue_t value) {
	struct CListNode *newNode = (struct CListNode*) calloc(1, sizeof(struct CListNode));

	if(newNode) {
		newNode->next = next;
		if(next) {
			newNode->prev = next->prev;
			if(newNode->prev)
				newNode->prev->next = newNode;
			next->prev = newNode;
		} else
			newNode->prev = NULL;
		newNode->value = value;
	}

	return newNode;
}

void clistDeleteNode(struct CListNode *node) {
	if(node) {
		if(node->prev)
			node->prev->next = node->next;
		if(node->next)
			node->next->prev = node->prev;
		free(node);
	}
}

void clistFree(struct CListNode *node) {
	struct CListNode *pnode = node->prev;
	while(pnode) {
		struct CListNode *prev = pnode->prev;
		free(pnode);
		pnode = prev;
	}

	struct CListNode *nnode = node->next;
	while(nnode) {
		struct CListNode *next = nnode->next;
		free(nnode);
		nnode = next;
	}

	free(node);
}

struct CListNode *clistGetNext(struct CListNode *node) { return node->next; }
struct CListNode *clistGetPrev(struct CListNode *node) { return node->prev; }

const struct CListNode *clistGetNextC(const struct CListNode *node) { return node->next; }
const struct CListNode *clistGetPrevC(const struct CListNode *node) { return node->prev; }

      listValue_t *clistGetValue (      struct CListNode *node) { return &node->value; };
const listValue_t *clistGetValueC(const struct CListNode *node) { return &node->value; };

void clistDump(const struct CListNode *node, int level) {
	if(!node)
		return;

	char tmpFileName[128] = "";

	snprintf(tmpFileName, sizeof(tmpFileName), P_tmpdir"/graph%d.dot", getpid());

	FILE *dot = fopen(tmpFileName, "w");

	fprintf(dot, "digraph G {\n");
	fprintf(dot, "splines=ortho;\n");
	fprintf(dot, "node [shape=box style=filled];\n");

	printLog(LOG_DEBUG, "prev = %p next = %p", node->prev, node->next);

	int prevCount = 0, nextCount = 0;

	for(const struct CListNode *index = node->next; index; index = index->next) {
		nextCount++;
		fprintf(dot, "NodeN%d[color=\"#00FF00\" fillcolor=\"#AAFFAA\" shape=record "
			   "label=\"{NodeN%d|%p|"LIST_FORMAT"|prev=%p|next=%p}\"];\n",
				nextCount, nextCount, index, index->value, index->prev, index->next);
	}

	for(const struct CListNode *index = node->prev; index; index = index->prev) {
		prevCount++;
		fprintf(dot, "NodeP%d[color=\"#FF0000\" fillcolor=\"#FFAAAA\" shape=record "
				"label=\"{NodeP%d|%p|"LIST_FORMAT"|prev=%p|next=%p}\"];\n",
				prevCount, prevCount, index, index->value, index->prev, index->next);
	}
	fprintf(dot, "Node0[shape=record label=\"{Node0|%p|"LIST_FORMAT"|prev=%p|next=%p}\"];\n",
				node, node->value, node->prev, node->next);

	if(nextCount)
		fprintf(dot, "Node0->NodeN1;\n");
	if(prevCount)
		fprintf(dot, "NodeP1->Node0;\n");
	for(int index = 1; index < nextCount; index++)
		fprintf(dot, "NodeN%d->NodeN%d;\n", index, index + 1);
	for(int index = 1; index < prevCount; index++)
		fprintf(dot, "NodeP%d->NodeP%d;\n", index + 1, index);

	fprintf(dot, "{ rank = same; Node0 ");
	for(int index = 1; index <= nextCount; index++)
		fprintf(dot, "NodeN%d ", index);
	for(int index = 1; index <= prevCount; index++)
		fprintf(dot, "NodeP%d ", index);
	fprintf(dot, "}\n");

	fprintf(dot, "}\n");

	fclose(dot);

	insertGraphLog(level, tmpFileName, "Classic list node=[%p] dump:", node);
	remove(tmpFileName);
}
