#include "files.h"

#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

void writeStrings(const char **lines, size_t lineCount, FILE *output) {
	assert(lines);
	assert(output);

	for(int line = 0; line < lineCount; line++) {
		if(*lines[line]) {
			fputs(lines[line], output);
			fputc('\n', output);
		}
	}
}

void writeStringsDirect(const char *data, size_t lineCount, FILE *output) {
	assert(data);

	for(size_t line = 0; line < lineCount; line++) {
		fputs(data, output);
		fputc('\n', output);

		while(!*data)
			data++;
	}
}

size_t getFileSize(FILE *file) {
	assert(file);

	fseek(file, 0L, SEEK_END);
	long size = ftell(file);

	assert(size >= 0);

	rewind(file);
	return (size_t) size;
}

char *readFile(FILE *file) {
	assert(file);

	size_t fileSize = getFileSize(file);
	char *data = (char*) calloc(fileSize + 1, sizeof(char));
	if(!data)
		return NULL;

	char *readPointer = data;
	size_t readed = 0;
	while((readed = fread(readPointer, sizeof(char), fileSize, file)) > 0)
		readPointer += readed;
	*readPointer = '\0';

	return data;
}

char *readTTY(FILE *file) {
	assert(file);
	size_t capicity = 4096;
	size_t readedAll = 0;

	char *data = (char*) malloc(capicity + 1);

	size_t readed = 0;

	while((readed = fread(data + readedAll, sizeof(char), capicity - readedAll, file)) > 0) {
		readedAll += readed;

		assert(readed <= capicity);

		if(readed < capicity)
			break;

		capicity *= 2;
		data = realloc(data, capicity + 1);
	}

	*(data + readedAll) = '\0';

	return data;
}

