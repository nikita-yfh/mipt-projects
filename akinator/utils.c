#include "utils.h"

#include <stdlib.h>

size_t getFileSize(FILE *file) {
	size_t prevPos = ftell(file);

	fseek(file, 0, SEEK_END);
	size_t fileSize = ftell(file);
	fseek(file, prevPos, SEEK_SET);

	return fileSize;
}

char *readFile(FILE *file) {
	size_t size = getFileSize(file);
	char *buf = (char*) calloc(1, size + 1);
	
	if(fread(buf, 1, size, file) != size) {
		free(buf);
		return NULL;
	}

	return buf;
}
