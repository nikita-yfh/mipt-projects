#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

/* struct EscapeCharacter { */
/* 	char escape; */
/* 	char normal; */
/* }; */

/* static const struct EscapeCharacter escapeCharacters[] = { */
/* 	{'a',  '\a'}, */
/* 	{'b',  '\b'}, */
/* 	{'e',  '\e'}, */
/* 	{'f',  '\f'}, */
/* 	{'n',  '\n'}, */
/* 	{'r',  '\r'}, */
/* 	{'t',  '\t'}, */
/* 	{'v',  '\v'}, */
/* 	{'0',  '\0'}, */
/* 	{'\\', '\\'}, */
/* 	{'\'', '\''}, */
/* 	{'\"', '\"'} */
/* }; */

/* static char getNormalCharacter(char c) { */
/* 	for(size_t index = 0; index < sizeof(escapeCharacters) / sizeof(*escapeCharacters); index++) */
/* 		if(c == escapeCharacters[index].escape) */
/* 			return escapeCharacters[index].normal; */
/* 	return '\0'; */
/* } */

/* static char getEscapedCharacter(char c) { */
/* 	for(size_t index = 0; index < sizeof(escapeCharacters) / sizeof(*escapeCharacters); index++) */
/* 		if(c == escapeCharacters[index].normal) */
/* 			return escapeCharacters[index].escape; */
/* 	return '\0'; */
/* } */

static int isOperator(char c) {
	const char *operators = "!@#$%^&*()+=-=\\|[{]};:?/,";
	for(const char *operator = operators; *operator; operator++)
		if(c == *operator)
			return 1;
	return 0;
}

static int isNumber(char c) {
	return isdigit(c) || c == '.';
}

static int isName(char c) {
	return isalpha(c) || c == '_';
}

struct Token *tokensCreate(char *line, struct SyntaxError *error) {
	struct Token *tokens = (struct Token*) calloc(strlen(line), sizeof(struct Token));

	struct Token *token = tokens;

	struct FilePosition filePosition = {1, 1};

	while(*line) {
		if(isOperator(*line)) {
			token->type = TOKEN_OPERATOR;
			token->op = *line;
			*(line++) = '\0';
			filePosition.column++;
			token++;
		} else if(isNumber(*line)) {
			char *tokenBegin = line;
			errno = 0;
			token->type = TOKEN_NUMBER;
			token->number = strtod(line, &line);
			token++;

			if(errno || tokenBegin == line) {
				error->type = ERROR_NUMBER_FORMAT;
				error->position = filePosition;
				free(tokens);
				return NULL;
			}
			filePosition.column += (int) (line - tokenBegin);
			while(tokenBegin < line)
				*(tokenBegin++) = '\0';
		} else if(*line == '"') {
			*(line++) = '\0';
			do {
				if(*line == '\0' || *line == '\n') {
					error->type = ERROR_UNTERMINATED_STRING;
					error->position = filePosition;
					free(tokens);
					return NULL;
				}

				filePosition.column++;
			} while(*(++line) != '"');

			token->type = TOKEN_STRING;
			token->text = line;
			token++;

			*(line++) = '\0';
		} else if(*line == '\n') {
			filePosition.line++;
			filePosition.column = 1;
			*(line++) = '\0';
		} else if(*line == ' ' || *line == '\t') {
			filePosition.column++;
			*(line++) = '\0';
		} else if(isName(*line)) {
			token->type = TOKEN_NAME;
			token->text = line;
			token++;

			while(isName(*line)) {
				line++;
				filePosition.column++;
			}
		}
	}
	token->type = TOKEN_EOF;
	return tokens;
}

