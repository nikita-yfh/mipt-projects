#ifndef ASM_H
#define ASM_H

#include <stdio.h>

struct AsmError {
	unsigned int line;
	unsigned int arg;
	const char *file;
	const char *message;
};

struct AsmInput {
	const char *inputFile;
	const char *outputFile;

	FILE *in;
	FILE *out;
};

int assembleFile(struct AsmInput *input, struct AsmError *error);

#endif
