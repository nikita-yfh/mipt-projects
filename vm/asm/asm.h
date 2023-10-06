#ifndef ASM_H
#define ASM_H

#include <stdio.h>

struct AsmError {
	unsigned int line;
	unsigned int arg;
	const char *file;
	const char *message;
};

int assembleFile(FILE *in, FILE *out, struct AsmError *error);

#endif
