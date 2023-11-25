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

	if(!currentChar)
		return (struct ProcessCharReturn) {MODE_SPACE, 0, 0};

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

void initTokens(struct Tokener *tokener) {
	assert(tokener);
	tokener->mode = MODE_SPACE;
}

int readNextToken(struct Tokener *tokener, FILE *file) {
	assert(tokener);
	assert(file);

	char *buffer = tokener->buffer;

	if(tokener->lastChar)
		(*buffer++) = tokener->lastChar;

	int currentChar = 0;
	while((currentChar = fgetc(file)) != EOF
			&& (size_t) (buffer - tokener->buffer) < sizeof(tokener->buffer)) {
		struct ProcessCharReturn result = processChar((char) currentChar, tokener->mode);
		printLog(LOG_INFO, "Result: char=%c mode=%d end=%d", result.fuckingChar, result.newMode, result.tokenEnd);
		tokener->lastChar = result.fuckingChar;
		tokener->mode = result.newMode;

		if(result.tokenEnd && buffer != tokener->buffer) {
			*buffer = '\0';
			return 0;
		}

		if(result.fuckingChar)
			*(buffer++) = result.fuckingChar;
	}

	if(!tokener->lastChar)
		return -1;

	*buffer = '\0';
	tokener->lastChar = 0;

	return 0;
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

