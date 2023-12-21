#include "syntaxError.h"
#include "log.h"

#include <string.h>

const char *errorDescription[] = {
	#define DEF_ERROR(type, text) text,
	#include "errors.h"
	#undef  DEF_ERROR
};

const char *getErrorDescription(const struct SyntaxError *error) {
	if(error->type < 0 || error->type >= ERROR_COUNT)
		return "Unknown error";
	return errorDescription[error->type];
}

void printError(const struct SyntaxError *error) {
	if(error->type != ERROR_NONE) {
		printLog(LOG_ERROR, "Error: %s", getErrorDescription(error));
		char spaces[1024];
		memset(spaces, ' ', (size_t) (error->position.column - 1));
		spaces[error->position.column - 1] = '^';
		spaces[error->position.column    ] = '\0';

		printLog(LOG_ERROR,   "%5d | %s", error->position.line, error->text);
		printLog(LOG_ERROR, "      | %s", spaces);
	}
	else 
		printLog(LOG_WTF, "No error?...");
}

