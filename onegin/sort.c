#include "sort.h"

#include <assert.h>
#include <string.h>

static int isUnusedCharacter(char c) {
	if(c > 0x08 && c <= 0x40)
		return 1;
	return 0;
}

static const char *endLine(const char *str) {
	while(*(str++));
	return str - 1;
}

static int strcmpEnd(const char *begin1, const char *begin2,
                      const char *end1,   const char *end2) {
	while(begin1 != end1 && begin2 != end2) {
		if(*end1 != *end2)
			return *end1 - *end2;

		end1--;
		end2--;
	}

	return 0;
}


static int compareStringsBegin(const void *a, const void *b) {
	const char *str1 = *((const char* const*) a);
	const char *str2 = *((const char* const*) b);

	while(isUnusedCharacter(*str1))
		str1++;
	while(isUnusedCharacter(*str2))
		str2++;
	
	return strcmp(str1, str2);
}

static int compareStringsPointer(const void *a, const void *b) {
	const char *str1 = *((const char* const*) a);
	const char *str2 = *((const char* const*) b);

	return (int) (str1 - str2);
}

static int compareStringsEnd(const void *a, const void *b) {
	const char *str1 = *((const char* const*) a);
	const char *str2 = *((const char* const*) b);

	const char *end1 = endLine(str1);
	const char *end2 = endLine(str2);

	while(str1 != end1 && isUnusedCharacter(*end1))
		end1--;
	while(str2 != end2 && isUnusedCharacter(*end2))
		end2--;

	return strcmpEnd(str1, str2, end1, end2);
}

void sortStrings(const char **lines, size_t lineCount, enum SortType type) {
	assert(lines);
	assert(type >= SORT_BEGIN && type <= SORT_POINTER);

	compareFunction_t *functions[] = {
		&compareStringsBegin,
		&compareStringsEnd,
		&compareStringsPointer
	};

	myqsort(lines, lineCount, sizeof(const char*), functions[type]);
}

static void swap(void *a, void *b, size_t size) {
	assert(size < 4096);
	char buf[4096] = {};

	memcpy(buf, a, size);
	memcpy(a,   b, size);
	memcpy(b, buf, size);
}

size_t partition(char *begin, char *end, size_t size, compareFunction_t *comparator) {
	char pivot[4096] = {};
	memcpy(pivot, begin, size);

	char *left  = begin;
	char *right = end;

	while(1) {
		while(left < right && comparator(left, pivot) < 0)
			left += size;
		while(left < right && comparator(pivot, right) < 0)
			right -= size;
		if(left != right && comparator(left, right) == 0) {
			left += size;
		}
		if(left < right)
			swap(left, right, size);
		else {
			assert(left >= begin && left <= end);
			return (size_t) (left - begin) / size;
		}
	}
}

typedef int compareFunction_t(const void*, const void*);

void myqsort(void *_data, size_t count, size_t size, compareFunction_t *comparator) {
	assert(size < 4096);
	char *data = (char*) _data;

	if(count <= 1);
	else if(count == 2) {

		if(comparator(data, data + size) > 0)
			swap(data, data + size, size);

	} else if(count == 3) {
		if(comparator(data,        data + size)     > 0)
			swap(data,             data + size,     size);

		if(comparator(data + size, data + size * 2) > 0)
			swap(data + size,      data + size * 2, size);

		if(comparator(data,        data + size * 2) > 0)
			swap(data,             data + size * 2, size);

	} else {
		size_t pivot = partition(data, data + size * (count - 1), size, comparator);

		myqsort(data, pivot, size, comparator);
		myqsort(data + size * (pivot + 1), count - pivot - 1, size, comparator);
	}
}

