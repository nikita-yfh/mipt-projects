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

struct Token {
	int type;
	const char *text;
};

struct Tokens {
	char *buffer;

	size_t count;
	struct Token *tokens;
};

#define tokenIsValid(token) ((token)->text != NULL)

struct Tokens tokensCreate(const char *line);
void tokensDelete(struct Tokens *tokens);

void writeQuotedString(const char *line, FILE *file);

#endif // TOKENER

