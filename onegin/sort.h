#ifndef SORT_H
#define SORT_H

#include <stdlib.h>

enum SortType {
	SORT_BEGIN,
	SORT_END,
	SORT_POINTER
};

void sortStrings(const char **lines, size_t lineCount, enum SortType type);

typedef int compareFunction_t(const void*, const void*);

void myqsort(void *data, size_t count, size_t size, compareFunction_t *comparator);

#endif

