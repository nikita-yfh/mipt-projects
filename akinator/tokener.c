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
			return (struct ProcessCharReturn) {MODE_SPACE, 0, 0};
		if(charIsEscape)
			return (struct ProcessCharReturn) {MODE_ESCAPE, 0, 0};
		return (struct ProcessCharReturn) {MODE_STRING, 0, currentChar};
	case MODE_ESCAPE: {
		char escape = getNormalCharacter(currentChar);
		if(escape)
			return (struct ProcessCharReturn) {MODE_STRING, 0, escape};
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

	tokens.tokens = (struct Token*) calloc(tokens.count + 1, sizeof(struct Token));

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

void writeQuotedString(const char *line, FILE *file) {
	fputc('"', file);

	while(*line) {
		char escaped = getEscapedCharacter(*line);
		if(escaped) {
			fputc('\\', file);
			fputc(escaped, file);
		} else
			fputc(*line, file);

		line++;
	}
	
	fputc('"', file);
}

