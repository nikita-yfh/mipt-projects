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

void cutLines(char *data) {
	while(*data != '\0') {
		if(*data == '\n' || *data == '\r')
			*data = '\0';
		data++;
	}
}

unsigned int countLines(const char *data) {
	assert(data);

	unsigned int count = 0;
	while(*data != '\0') {
		if(*data == '\n') {
			data++;
			while(*data == '\n' || *data == '\r')
				data++;
			count++;
		} else
			data++;
	}

	return count;
}

const char *skipChar(const char *str, char c) {
	assert(*str == c);
	return str + 1;
}

const char *skipBlanks(const char *str) {
	while(*str == ' ')
		str++;
	return str;
}

unsigned long strlenToChar(const char *str, char c) {
	const char *space = strchr(str, c);
	if(!space)
		return strlen(str);

	assert(space >= str);

	return (unsigned int) (space - str);
}

const char *skipNull(const char *str) {
	assert(str);
	return str + strlen(str) + 1;
}
