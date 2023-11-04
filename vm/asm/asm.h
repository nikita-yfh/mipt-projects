#ifndef ASM_H
#define ASM_H

#include <stdio.h>
#include <stdbool.h>

struct AsmError {

	unsigned int line;
	unsigned int column;
	const char *file;

	const char *message;
};

struct AsmInput {
	const char *inputFile;
	const char *outputFile;

        // TODO: Why are you storing both file names and FILE*?

        //       FILE* is a lot more abstract thing, for example
        //       it can serve as an abstraction for stdin/stdout,
        //       which would naturally enable your compiler to
        //       compile code from stdin.

        //       And it's it not even that, I just find storing
        //       the thing that you get and the thing that you
        //       use to get it both at in the same struct at the
        //       same time weird and unnecessary.

	FILE *in;
	FILE *out;

	bool skipHeader;
};

int assembleFile(struct AsmInput *input, struct AsmError *error);

#endif
