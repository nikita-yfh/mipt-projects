#include "parser.h"
#include "log.h"

#include <assert.h>
#include <string.h>

static struct BinaryTreeNode *getUnary(const struct Token **token, struct SyntaxError *error);
static struct BinaryTreeNode *getExpr(const struct Token **token, struct SyntaxError *error);

static struct BinaryTreeNode *getNumber(const struct Token **token, struct SyntaxError*) {
	assert(token);
	assert(*token);

	if((*token)->type != TOKEN_NUMBER)
		return NULL;

	return btreeNewConst(((*token)++)->number);
}

static struct BinaryTreeNode *getVariable(const struct Token **token, struct SyntaxError *error) {
	assert(token);
	assert(*token);

	if((*token)->type != TOKEN_NAME)
		return NULL;

	if(strlen((*token)->text) > 1) {
		error->type = ERROR_VAR_LONG;
		error->position = (*token)->position;
		return NULL;
	}

	return btreeNewVariable(*((*token)++)->text);
}

static struct BinaryTreeNode *getHard(const struct Token **token, struct SyntaxError *error) {
	assert(token);
	assert(*token);

	if((*token)->type == TOKEN_OPERATOR) {
		const struct Operator *operator = operatorGet((*token)->operator);
		assert(operator);

		if(operator->type == OPERATOR_LBRACKET) {
			(*token)++;
			struct BinaryTreeNode *node = getExpr(token, error);

			if(!node)
				return NULL;

			if((*token)->type == TOKEN_OPERATOR) {
				operator = operatorGet((*token)->operator);
				if(operator->type == OPERATOR_RBRACKET) {
					(*token)++;
					return node;
				}
			}

			error->position = (*token)->position;
			error->type = ERROR_MISSING_BRACKET;
			return NULL;
		}
	}

	switch((*token)->type) {
		case TOKEN_NUMBER: return getNumber(token, error);
		case TOKEN_NAME:   return getVariable(token, error);
	}

	error->type = ERROR_INVALID_TOKEN;
	error->position = (*token)->position;
	return NULL;
}

static struct BinaryTreeNode *getUnary(const struct Token **token, struct SyntaxError *error) {
	assert(token);
	assert(*token);

	if((*token)->type == TOKEN_OPERATOR) {
		const struct Operator *operator = operatorGet((*token)->operator);
		assert(operator);

		if(operator->type & OPERATOR_UNARY) {
			struct BinaryTreeNode *node = btreeNewOperator((*token)->operator);
			(*token)++;

			node->left = getUnary(token, error);
			node->left->parent = node;

			return node;
		}
	}

	return getHard(token, error);
}

static struct BinaryTreeNode *getBinary(const struct Token **token, int deep, struct SyntaxError *error) {
	assert(token);
	assert(*token);

	if(deep > OPERATOR_MAX_PRIORITY)
		return getUnary(token, error);

	struct BinaryTreeNode *a = getBinary(token, deep + 1, error);
	if(!a)
		return NULL;

	while((*token)->type == TOKEN_OPERATOR && 
			operatorGet((*token)->operator)->binaryPriority == deep) {
		struct BinaryTreeNode *operator = btreeNewOperator((*token)->operator);
		(*token)++;

		struct BinaryTreeNode *b = getBinary(token, deep + 1, error);
		if(!b)
			return NULL;

		operator->left  = a;
		operator->right = b;

		operator->left->parent  = operator;
		operator->right->parent = operator;

		a = operator;
	}

	return a;
}

static struct BinaryTreeNode *getExpr(const struct Token **token, struct SyntaxError *error) {
	return getBinary(token, 0, error);
}

struct BinaryTreeNode *parseString(const char *str, struct SyntaxError *error) {
	error->text = str;

	char *buffer = strdup(str);

	struct Token *tokens = tokensCreate(buffer, error);

	const struct Token *token = tokens;

	if(!tokens)
		return NULL;

	struct BinaryTreeNode *node = getExpr(&token, error);
	if(!node)
		return NULL;

	if(!token || token->type != TOKEN_EOF) {
		error->position = token->position;
		error->type = ERROR_EOF;
		return NULL;
	}

	free(tokens);

	return node;
}

