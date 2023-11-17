#ifndef CLIST_H
#define CLIST_H

#include <stdint.h>
#include <stdlib.h>
#include "config.h"

struct CListNode {
	listValue_t value;
	struct CListNode *prev;
	struct CListNode *next;
};

struct CListNode *clistInsertAfter (struct CListNode *prev, listValue_t value);
struct CListNode *clistInsertBefore(struct CListNode *next, listValue_t value);

void clistDeleteNode(struct CListNode *node);

void clistFree(struct CListNode *node);

struct CListNode *clistGetNext(struct CListNode *node);
struct CListNode *clistGetPrev(struct CListNode *node);

const struct CListNode *clistGetNextC(const struct CListNode *node);
const struct CListNode *clistGetPrevC(const struct CListNode *node);

listValue_t *clistGetValue(struct CListNode *node);
const listValue_t *clistGetValueC(const struct CListNode *node);

void clistDump(const struct CListNode *node, int level);

#endif

