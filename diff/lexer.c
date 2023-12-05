#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

struct EscapeCharacter {
	char escape;
	char normal;
};

static const struct EscapeCharacter escapeCharacters[] = {
	{'a',  '\a'},
	{'b',  '\b'},
	{'e',  '\e'},
	{'f',  '\f'},
	{'n',  '\n'},
	{'r',  '\r'},
	{'t',  '\t'},
	{'v',  '\v'},
	{'0',  '\0'},
	{'\\', '\\'},
	{'\'', '\''},
	{'\"', '\"'}
};

static char getNormalCharacter(char c) {
	for(size_t index = 0; index < sizeof(escapeCharacters) / sizeof(*escapeCharacters); index++)
		if(c == escapeCharacters[index].escape)
			return escapeCharacters[index].normal;
	return '\0';
}

static char getEscapedCharacter(char c) {
	for(size_t index = 0; index < sizeof(escapeCharacters) / sizeof(*escapeCharacters); index++)
		if(c == escapeCharacters[index].normal)
			return escapeCharacters[index].escape;
	return '\0';
}

struct Token *tokensCreate(char *line) {
	struct Token *tokens = (struct Token*) calloc(strlen(line), sizeof(struct Token));

	struct Token *token = tokens;

	struct FilePosition filePosition = {1, 1};

	while(*line) {
		switch(*line) {
			case '\0':
				break;
			case '!': case '@': case '#': case '$':
			case ';': case ':': case '%': case '^':
			case '&': case '?': case '*': case '(':
			case ')': case '-': case '_': case '+':
			case '=': case '\\':case '/': case '|':
			case '{': case '}': case '[': case ']':
			case '~': case '`':
				token->type = TOKEN_OPERATOR;
				token->op = *line;

				token++;
				break;
			case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8':
			case '9': case '0': case '.':
				break;
			case '\n':
				filePosition.line++;
				filePosition.column = 1;
				/* fall through */
			case ' ': case '\t':
				break;
		}
		filePosition.column++;
	}
	return tokens;
}

