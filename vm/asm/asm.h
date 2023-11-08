#ifndef ASM_H
#define ASM_H

#include <stdio.h>
#include <stdbool.h>

struct AsmError {
	const char *line;
	unsigned int lineNumber;
	unsigned int column;

	const char *fileName;

	const char *message;
};

struct AsmInput {
	const char *inputFile;
	const char *outputFile;

	FILE *in;
	FILE *out;

	bool skipHeader;
};

// TODO: if these are meant to be constructors and destructors of AsmInput,
//       than name them like that. Since AsmInput is not just list of "files", 
//       openFiles doesn't make a lot of sense.
int    openFiles(struct AsmInput *input);
int   closeFiles(struct AsmInput *input);
int assembleFile(struct AsmInput *input);

#endif
