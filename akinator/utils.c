#include "utils.h"

#include <stdlib.h>

unsigned long int getFileSize(FILE *file) {
	long int prevPos = ftell(file);

	fseek(file, 0, SEEK_END);
	long int fileSize = ftell(file);
	fseek(file, prevPos, SEEK_SET);

	return (unsigned long int) fileSize;
}

char *readFile(FILE *file) {
	unsigned long int size = getFileSize(file);
	char *buf = (char*) calloc(1, size + 1);
	
	if(fread(buf, 1, size, file) != size) {
		free(buf);
		return NULL;
	}

	return buf;
}
