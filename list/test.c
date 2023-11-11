#include "list.h"

int main() {
	struct List list;
	listCreate(&list);
	listInsertNodeAfter(&list, listGetHead(&list), 42);
	listDelete(&list);
	return 0;
}
