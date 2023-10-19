#ifndef DISASM_H
#define DISASM_H

#include <stdio.h>

struct DisasmError {
	unsigned int instruction;
	const char *file;
	const char *message;
};

struct DisasmInput {
	const char *inputFile;
	const char *outputFile;

	FILE *in;
	FILE *out;

	int otherNumberFormat;
};

int disassembleFile(struct DisasmInput *input, struct DisasmError *error);

#endif
