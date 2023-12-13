#ifndef TOKENER_H
#define TOKENER_H

#include <stdlib.h>
#include <stdio.h>

#include "syntaxError.h"

enum {
	TOKEN_EOF = -1,

	TOKEN_NONE,  // for internal usage
	TOKEN_OPERATOR,
	TOKEN_KEYWORD,
	TOKEN_NAME,
	TOKEN_STRING,
	TOKEN_NUMBER
};

typedef unsigned int operation_t;

struct Token {
	int type;

	struct FilePosition position;

	union {
		operation_t operation;
		const char *text;
		double number;
	};
};

struct Token *tokensCreate(char *line, struct SyntaxError *error);

#endif // TOKENER

