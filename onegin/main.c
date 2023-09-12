#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

size_t getFileSize(FILE *file);
unsigned int countLines(const char *data);
unsigned int cutLines(char *data, const char ***lines);
void readFile(char *data, size_t size, FILE *file);
int compareStringsBegin(const void *a, const void *b);
void processOnegin(FILE *input, FILE *output);
void writeStrings(const char **lines, FILE *output);

int main(int argc, const char*const*argv) {
	if(argc != 3) {
		fprintf(stderr, "Usage: %s <input> <output>\n", argv[0]);
		return -1;
	}
	const char *input = argv[1];
	const char *output = argv[2];

	FILE *in = fopen(input, "rb");
	if(!in) {
		perror("Failed to open input file");
		return -1;
	}

	FILE *out = fopen(output, "w");
	if(!out) {
		perror("Failed to open output file");
		fclose(in);
		return -1;
	}

	processOnegin(in, out);

	fclose(in);
	fclose(out);

	return 0;
}

void processOnegin(FILE *input, FILE *output) {
	assert(input);
	assert(output);

	size_t fileSize = getFileSize(input);
	char *data = (char*) calloc(fileSize + 1, sizeof(char));
	readFile(data, fileSize, input);

	const char **lines = NULL;
	size_t lineCount = cutLines(data, &lines);

	qsort(lines, lineCount, sizeof(const char*), compareStringsBegin);
	writeStrings(lines, output);

	free(lines);
	free(data);
}

void writeStrings(const char **lines, FILE *output) {
	assert(lines);
	assert(output);

	while(*lines) {
		fputs(*(lines++), output);
		fputc('\n', output);
	}
}

int compareStringsBegin(const void *a, const void *b) {
	const char *str1 = *((const char**) a);
	const char *str2 = *((const char**) b);

	while(*str1 && *str1 >= 0x8 && *str1 <= 0x2F)
		str1++; // skip not usable characters
	while(*str2 && *str2 >= 0x8 && *str2 <= 0x2F)
		str2++;
	
	return strcmp(str1, str2);
}

size_t getFileSize(FILE *file) {
	assert(file);

	fseek(file, 0L, SEEK_END);
	long size = ftell(file);

	assert(size >= 0);

	rewind(file);
	return (size_t) size;
}

void readFile(char *data, size_t size, FILE *file) {
	assert(data);
	assert(file);

	char *readPointer = data;
	size_t readed = 0;
	while((readed = fread(readPointer, sizeof(char), size, file)) > 0)
		readPointer += readed;
	*readPointer = '\0';
}

unsigned int cutLines(char *data, const char ***linesPointer) {
	assert(data);
	assert(linesPointer);

	unsigned int lineCount = countLines(data);
	const char **lines = (const char **) calloc(lineCount + 1, sizeof(char*));
	*linesPointer = lines;

	while(*data) {
		if(*data == '\n' || *data == '\r') {
			while(*data == '\n' || *data == '\r') {
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

	unsigned int count = 0;
	while(*data != '\0') {
		if(*data == '\n' || *data == '\r') {
			while(*data == '\n' || *data == '\r')
				data++;
			count++;
		} else
			data++;
	}

	return count;
}
