#include "tokener.h"
#include "log.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

static int isoperator(char c) {
	const char *operators = "!:;,./$%^&?*()-+=<>{}[]|~";
	while(*operators)
		if(*(operators++) == c)
			return 1;
	return 0;
}

struct EscapeCharacter {
	char escape;
	char result;
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

static const struct EscapeCharacter *getEscapeCharacter(char c) {
	for(size_t index = 0; index < sizeof(escapeCharacters) / sizeof(*escapeCharacters); index++)
		if(c == escapeCharacters[index].escape)
			return &escapeCharacters[index];
	return NULL;
}

struct ProcessCharReturn {
	int newMode;
	int tokenEnd;
	char fuckingChar;
};

static struct ProcessCharReturn processChar(char currentChar, int mode) {

	int charIsDigit      = isdigit(currentChar);
	int charIsNumber     = charIsDigit || currentChar == '.' || currentChar == '-';
	int charIsAlpha      = isalpha(currentChar) || currentChar == '_';
	int charIsAlphaDigit = charIsDigit || charIsAlpha;
	int charIsSpace      = isspace(currentChar) || currentChar == '\0';
	int charIsOperator   = isoperator(currentChar);
	int charIsEscape     = (currentChar == '\\');
	int charIsQuote      = (currentChar == '\"');

	switch(mode) {
	case MODE_STRING:
		if(charIsQuote)
			return (struct ProcessCharReturn) {MODE_SPACE, 1, 0};
		if(charIsEscape)
			return (struct ProcessCharReturn) {MODE_ESCAPE, 0, 0};
		return (struct ProcessCharReturn) {MODE_STRING, 0, currentChar};
	case MODE_ESCAPE: {
		const struct EscapeCharacter *escape = getEscapeCharacter(currentChar);
		if(escape)
			return (struct ProcessCharReturn) {MODE_STRING, 0, escape->result};
		return (struct ProcessCharReturn) {MODE_ERROR, 0, 0};
	}
	case MODE_NAME:
		if(charIsAlphaDigit)
			return (struct ProcessCharReturn) {MODE_NAME, 0, currentChar};
		break;
	case MODE_NUMBER:
		if(charIsNumber)
			return (struct ProcessCharReturn) {MODE_NUMBER, 0, currentChar};
		break;
	default:
		break;
	}

	// reset parsing
	if(charIsSpace)
		return (struct ProcessCharReturn) {MODE_SPACE,    0, 0};
	if(charIsQuote)
		return (struct ProcessCharReturn) {MODE_STRING,   1, 0};
	if(charIsOperator)
		return (struct ProcessCharReturn) {MODE_OPERATOR, 1, currentChar};
	if(charIsNumber)
		return (struct ProcessCharReturn) {MODE_NUMBER,   1, currentChar};
	if(charIsAlpha)
		return (struct ProcessCharReturn) {MODE_NAME,     1, currentChar};

	return     (struct ProcessCharReturn) {MODE_ERROR,    0, 0};
}

static size_t tokensCount(const char *line) {
	size_t tokenCount = 0; 
	int mode = MODE_SPACE;

	do {
		struct ProcessCharReturn result = processChar(*line, mode);
		mode = result.newMode;
		if(result.tokenEnd)
			tokenCount++;
	} while(*(line++));

	return tokenCount;
}

struct Tokens tokensCreate(const char *line) {
	size_t maxLen = strlen(line) * 2 + 1;

	struct Tokens tokens = {};
	tokens.buffer = (char*) calloc(1, maxLen);
	tokens.count = tokensCount(line); 

	tokens.tokens = (struct Token*) calloc(tokens.count, sizeof(struct Token));

	struct Token *currentToken = tokens.tokens;
	char *buffer = tokens.buffer;
	int mode = MODE_SPACE;

	do {
		struct ProcessCharReturn result = processChar(*line, mode);
		mode = result.newMode;

		if(result.tokenEnd) {
			*(buffer++) = '\0';
			currentToken->text = buffer;
			currentToken->type = mode;
			currentToken++;
		}

		if(result.fuckingChar)
			*(buffer++) = result.fuckingChar;

	} while(*(line++));

	return tokens;
}

void tokensDelete(struct Tokens *tokens) {
	if(!tokens)
		return;
	free(tokens->tokens);
	free(tokens->buffer);
}

