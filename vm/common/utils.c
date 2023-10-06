#include "utils.h"

#include <stdio.h>
#include <assert.h>
#include <ctype.h>

int stricmp(const char *a, const char *b) {
	while(*a && *b && *a == *b) {
		*a++;
		*b++;
	}
	return tolower(*a) - tolower(*b);
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


