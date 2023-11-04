#ifndef ASM_H
#define ASM_H

#include <stdio.h>
        // TODO: this seems like an extractable thing as it
        // unambiguously represents location in source code:
	unsigned int line;
	unsigned int offset;
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

        // TODO: for bools use bool from stdbool.h or builtin
        //       bool from C++ (We allow it here if you don't mind),
        //       it conveys intent better and it's free!
	int needHeader;

        // TODO: Also, what are the reason to not include header?
        //       And what it really is. Add a comment with more info!
};

int assembleFile(struct AsmInput *input, struct AsmError *error);

#endif
