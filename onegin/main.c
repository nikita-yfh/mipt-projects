#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include "sort.h"
#include "utils.h"
#include "files.h"

void processOnegin(FILE *input, FILE *output);

int main(int argc, const char* const* argv) {
	FILE *input = NULL, *output = NULL;

	if(argc == 3) {
		input = fopen(argv[1], "rb");
		if(!input)
			perror("Failed to open input file");

		output = fopen(argv[2], "w");
		if(!output)
			perror("Failed to open output file");

	} else if(argc == 2 && strcmp(argv[1], "--") == 0) {
		input  = stdin;
		output = stdout;
	} else {
		fprintf(stderr, "Usage: %s <input> <output> [--]\n", argv[0]);
		return -1;
	}

	if(input && output)
		processOnegin(input, output);

	if(input)
		fclose(input);
	if(output)
		fclose(output);

	return 0;
}

int intComparator(void *a, void *b) {
	return *((int*)a) - *((int*)b);
}

void processOnegin(FILE *input, FILE *output) {
	assert(input);
	assert(output);

	char *data = NULL;
	if(input == stdin)
		data = readTTY(input);
	else
		data = readFile(input);

	const char** lines = NULL;
	size_t lineCount = cutLines(data, &lines);

	fputs("******************************************************************************************************\n", output);
	sortStrings(lines, lineCount, SORT_BEGIN);
	writeStrings(lines, lineCount, output);
	//fputs("******************************************************************************************************\n", output);
	//sortStrings(lines, lineCount, SORT_END);
	//writeStrings(lines, lineCount, output);
	fputs("******************************************************************************************************\n", output);
	writeStringsDirect(lines, lineCount, output);

	free(lines);
	free(data);
}

