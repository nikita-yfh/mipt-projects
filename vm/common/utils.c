#include "utils.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

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

unsigned int cutLines(char *data) {
	unsigned int lines = 1;
	while(*data != '\0') {
		if(*data == '\n' || *data == '\r') {
			if(*data == '\n')
				lines++;
			*data = '\0';
		}
		data++;
	}
	return lines;
}

const char *skipChar(const char *str, char c) {
	assert(*str == c);
	return str + 1;
}

const char *skipBlanks(const char *str) {
	while(*str == ' ' || *str == '\t')
		str++;
	return str;
}

// TODO: naming seems confusing. Maybe distanceToChar?
unsigned long strlenToChar(const char *str, char c) {
	const char *space = strchr(str, c);
	if(!space)
		return strlen(str);

	assert(space >= str);

	return (unsigned int) (space - str);
}

// TODO: this name seems a bit confusing too, maybe skipString?
const char *skipNull(const char *str) {
	assert(str);
	return str + strlen(str) + 1;
}
