#ifndef TOKENER_H
#define TOKENER_H

#include <stdlib.h>
#include <stdio.h>

enum {
	MODE_ERROR = -1,
	MODE_SPACE,
	MODE_NAME,
	MODE_NUMBER,
	MODE_OPERATOR,
	MODE_STRING,
	MODE_ESCAPE,
};

struct Tokener {
	char buffer[4096];
	int mode;
	char lastChar;
};

void initTokens(struct Tokener *tokener);
int readNextToken(struct Tokener *tokener, FILE *file);

void writeQuotedString(const char *line, FILE *file);

#endif // TOKENER

