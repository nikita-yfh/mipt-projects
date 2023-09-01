#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include "mystring.h"

void *mymemset(void *s, int c, size_t n) {
	void *ret = s;
	while(n --> 0)
		*((unsigned char*) (s++)) = c;
	return ret;
}


int mystrcmp(const char *s1, const char *s2) {
	while(*s1 && *s2 && *s1 == *s2) {
		s1++;
		s2++;
	}
	return *s1 - *s2;
}

// for internal usage, compare full s2 to s1 begin
static int mystrcmp_begin(const char *s1, const char *s2) {
	while(*s1 && *s2) {
		if(*s1 != *s2)
			return *s1 - *s2;
		s1++;
		s2++;
	}
	return (*s2) ? (*s2) : 0;
}

size_t mystrlen(const char *str) {
	size_t size = 0;
	while(*(str++))
		size++;
	return size;
}

char *mystrcat(char *dest, const char *src) {
	assert(dest);
	assert(src);
	assert(dest != src);

	char *begin = dest;
	while(*begin)
		begin++;

	while(*src)
		*(begin++) = *(src++);
	return dest;
}

char *mystrncat(char *dest, const char *src, size_t num) {
	assert(dest);
	assert(src);
	assert(dest != src);

	char *end = dest + num;
	char *begin = dest;
	while(*begin)
		begin++;

#ifndef NDEBUG
	size_t srcnum = mystrlen(src);
	assert(end < src || src + srcnum < dest);
#endif

	while(*src && begin < end - 1)
		*(begin++) = *(src++);
	*begin = 0; // remember null-terminator
	return dest;
}

char *mystrcpy(char *dest, const char *src) {
	char *ret = dest;

	assert(dest);
	assert(src);
	assert(dest != src);

	while(*src)
		*(dest++) = *(src++);

	return ret;
}

char *mystrncpy(char *dest, const char *src, size_t num) {
	char *ret = dest;

	assert(dest);
	assert(src);
	assert(dest != src);

	size_t counter = 0;
	while(counter++ != num && *src)
		*(dest++) = *(src++);

	return ret;
}

char *mystrstr1(const char *haystack, const char *needle) {
	assert(haystack);
	assert(needle);
	assert(haystack != needle);
	assert(*needle);

	while(*haystack) {
		if(mystrcmp_begin(haystack, needle) == 0)
			return (char*) haystack;
		haystack++;
	}
	return NULL;
}

char *mystrstr2(const char *haystack, const char *needle) {
	assert(haystack);
	assert(needle);
	assert(haystack != needle);
	assert(*needle);

	long hhash = 0; 
	long nhash = 0;

	size_t len = mystrlen(needle);
	
	for(unsigned int index = 0; index < len; index++) {
		hhash += haystack[index];
		nhash += needle[index];
	}

	const char *end = haystack + len;
	while(*end) {
		if(hhash == nhash) {
			if(mystrcmp_begin(haystack, needle) == 0)
				return (char*) haystack;
		}
		hhash -= *(haystack++);
		hhash += *(end++);
	}
		
	return NULL;
}

char *mystrstr3(const char *_haystack, const char *_needle) {
	assert(_haystack);
	assert(_needle);
	assert(_haystack != _needle);
	assert(*_needle);

	size_t nlen = mystrlen(_needle);
	size_t hlen = mystrlen(_haystack);

	const unsigned char *haystack = (const unsigned char*) _haystack;
	const unsigned char *needle = (const unsigned char*) _needle;

	size_t shifts[UINT8_MAX + 1];
	mymemset(shifts, 0xFF, sizeof(shifts));

	for(unsigned int index = 0; index < nlen; index++)
		shifts[needle[index]] = index;

	size_t index = nlen;
	while(index < hlen) {
		const unsigned char *hend = haystack + nlen - 1;
		const unsigned char *nend = needle + nlen - 1;

		printf("%lu %c %c\n", index, *hend, *haystack);

		while(hend != haystack && *hend == *nend) {
			hend--;
			nend--;
		}

		if(hend == haystack)
			return (char*) haystack;

		size_t shift = (shifts[*hend] == ULONG_MAX) ? (hend - haystack + 1) : (nlen - 1 - shifts[*hend]);

		printf("%lu %lu %c\n", shift, index, *hend);

		index += shift;
		haystack += shift;
	}
	return NULL;
}
