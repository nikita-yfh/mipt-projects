#ifndef SYNTAXERR_H
#define SYNTAXERR_H

struct FilePosition {
	int line;
	int column;
};

struct SyntaxError {
	struct FilePosition position;
	int type;
};

enum ErrorType {
	#define DEF_ERROR(type, text) type,
	#include "errors.h"
	#undef  DEF_ERROR

	ERROR_COUNT
};

const char *getErrorDescription(struct SyntaxError *error);

#endif // SYNTAXERR_H
