#include "utils.h"

#include <stdio.h>
#include <assert.h>
#include <ctype.h>

int stricmp(const char *a, const char *b) {
	while(*a && *b && tolower(*a) == tolower(*b)) {
		a++;
		b++;
	}
	return tolower(*a) - tolower(*b);
}

size_t getFileSize(FILE *file) {
	assert(file);

	long oldPosition = ftell(file);

	fseek(file, 0L, SEEK_END);
	long size = ftell(file);

	assert(size >= 0);

	fseek(file, oldPosition, SEEK_SET);
	return (size_t) size;
}

size_t countStrings(FILE *file) {
	assert(file);

	long oldPosition = ftell(file);

	size_t lines = 0;
	while(!feof(file))
		if(fgetc(file) == '\n')
			lines++;

	fseek(file, oldPosition, SEEK_SET);

	return lines;
}


