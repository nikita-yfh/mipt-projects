#ifndef TOKENER_H
#define TOKENER_H

#include <stdlib.h>

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

struct Tokens tokensCreate(const char *line);
void tokensDelete(struct Tokens *tokens);

#endif // TOKENER

