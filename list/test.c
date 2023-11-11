#include "list.h"
#include <assert.h>
#include <stdio.h>
#include "log.h"

int main() {
	struct List list;
	listCreate(&list);

	listDump(&list);

	listInsertNodeAfter(&list, listGetHead(&list), 42);
	listDump(&list);
	listInsertNodeAfter(&list, listGetHead(&list), 32);
	listDump(&list);
	listInsertNodeAfter(&list, listGetHead(&list), 22);
	listDump(&list);

	listDelete(&list);
	return 0;
}
