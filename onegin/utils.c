#include "utils.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

unsigned int cutLines(char *data, const char ***linesPointer) {
	assert(data);
	assert(linesPointer);

	unsigned int lineCount = countLines(data);
	const char **lines = (const char **) calloc(lineCount + 1, sizeof(char*));
	*linesPointer = lines;

	*(lines++) = data;

	while(*data) {
		if(*data == '\n') {
			*data = '\0';
			data++;

			if(*data == '\r') {
				*data = '\0';
				data++;
			}

			*(lines++) = data;
		} else
			data++;
	}

	return lineCount;
}

unsigned int countLines(const char *data) {
	assert(data);

	unsigned int count = 1;
	while(*data != '\0') {
		if(*data == '\n') {
			data++;
			if(*data == '\r')
				data++;
			count++;
		} else
			data++;
	}

	return count;
}

