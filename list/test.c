#include "list.h"
#include <assert.h>
#include "log.h"

int main() {
	struct List list;
	listCreate(&list);

	listDump(&list);

	listInsertNodeAfter(&list, listGetHead(&list), 42);
	listInsertNodeAfter(&list, listGetHead(&list), 32);
	listInsertNodeAfter(&list, listGetHead(&list), 22);

	printf("%d\n", *listGetValueC(&list, listGetHead(&list)));
	printf("%d\n", *listGetValueC(&list, listGetTail(&list)));

	listDelete(&list);
	return 0;
}
