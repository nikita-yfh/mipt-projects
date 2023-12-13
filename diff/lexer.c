#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

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

static int tokenCheckKeywords(struct Token *token, const char *text, size_t textLength) {

	const char *operators[] = {
		#define DEF_OPERATION(id, str, type, priority, func) str,
		#include "operations.h.gen"
		#undef DEF_OPERATION
	};

	for(unsigned i = 0; i < (sizeof operators / sizeof *operators); i++) {
		if(strlen(operators[i]) != textLength)
			continue;
		if(strncmp(text, operators[i], textLength) != 0)
			continue;
		token->type = TOKEN_OPERATOR;
		token->operation = i;
		return 1;
	}

	return 0;
}

static char *clearLine(char *begin, const char *end) {
	while(begin < end)
		*(begin++) = '\0';
	return begin;
}

struct Token *tokensCreate(char *line, struct SyntaxError *error) {
	struct Token *tokens = (struct Token*) calloc(strlen(line), sizeof(struct Token));

	struct Token *token = tokens;

	struct FilePosition filePosition = {1, 1};

	while(*line) {
		if(isOperator(*line)) {
			char *tokenBegin = line;

			while(isOperator(*line++))
				filePosition.column++;

			if(!tokenCheckKeywords(token, tokenBegin, (size_t) (line - token->text)))
				return NULL;
			token->position = filePosition;
			token++;

			clearLine(tokenBegin, line);

		} else if(isNumber(*line)) {
			char *tokenBegin = line;
			errno = 0;
			token->type = TOKEN_NUMBER;
			token->number = strtod(line, &line);
			token->position = filePosition;
			token++;

			if(errno || tokenBegin == line) {
				error->type = ERROR_NUMBER_FORMAT;
				error->position = filePosition;
				free(tokens);
				return NULL;
			}
			filePosition.column += (int) (line - tokenBegin);
			clearLine(tokenBegin, line);
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
			token->position = filePosition;
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
			/* token->type = TOKEN_NAME; */
			/* token->position = filePosition; */
			/* token->text = line; */
			/* token++; */

			/* while(isName(*line)) { */
			/* 	line++; */
			/* 	filePosition.column++; */
			/* } */

			/* if(!tokenCheckKeywords(token, (size_t) (line - token->text), error)) */
			/* 	return NULL; */
		}
	}
	token->type = TOKEN_EOF;
	return tokens;
}

