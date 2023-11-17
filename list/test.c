#include "list.h"
#include "clist.h"
#include <assert.h>
#include <stdio.h>
#include "log.h"

static void listTest() {
	printLog(LOG_INFO, "LIST TEST");

	struct List list;
	listCreate(&list);

	listDump(&list, LOG_VERBOSE);

	listInsertNodeBefore(&list, listGetHead(&list), 42);
	listDump(&list, LOG_VERBOSE);
	listInsertNodeBefore(&list, listGetHead(&list), 32);
	listDump(&list, LOG_VERBOSE);
	for(int i = 0; i < 20; i++)
		listInsertNodeBefore(&list, listGetHead(&list), 22);
	listDump(&list, LOG_VERBOSE);
	listDeleteNode(&list, listGetTail(&list));
	listDump(&list, LOG_VERBOSE);

	listDelete(&list);
}

static void clistTest() {
	printLog(LOG_INFO, "CLASSIC LIST TEST");

	struct CListNode *node = clistInsertAfter(NULL, 22);
	clistDump(node, LOG_VERBOSE);

	clistInsertBefore(node, 11);
	clistDump(node, LOG_VERBOSE);

	clistInsertBefore(node, 3);
	clistDump(node, LOG_VERBOSE);

	clistInsertAfter(node, 445);
	clistDump(node, LOG_VERBOSE);

	struct CListNode *index = clistInsertAfter(node, 78);
	clistDump(node, LOG_VERBOSE);

	clistDeleteNode(clistGetNext(index));
	clistDump(node, LOG_VERBOSE);

	clistFree(node);
}

int main() {
	listTest();
	clistTest();

	return 0;
}
