#include "list.h"
#include <assert.h>
#include <stdio.h>
#include "log.h"

int main() {
	struct List list;
	listCreate(&list);

	listDump(&list, LOG_VERBOSE);

	listInsertNodeBefore(&list, listGetHead(&list), 42);
	listDump(&list, LOG_VERBOSE);
	listInsertNodeBefore(&list, listGetHead(&list), 32);
	listDump(&list, LOG_VERBOSE);
	listIndex_t index = listInsertNodeBefore(&list, listGetHead(&list), 22);
	listDump(&list, LOG_VERBOSE);
	listDeleteNode(&list, listGetNextNode(&list, index));
	listDump(&list, LOG_VERBOSE);

	listDelete(&list);
	return 0;
}
