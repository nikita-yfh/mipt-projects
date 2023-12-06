#include "syntaxError.h"

const char *errorDescription[] = {
	#define DEF_ERROR(type, text) text,
	#include "errors.h"
	#undef  DEF_ERROR
};

const char *getErrorDescription(struct SyntaxError *error) {
	if(error->type < 0 || error->type >= ERROR_COUNT)
		return "Unknown error";
	return errorDescription[error->type];
}
