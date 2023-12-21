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

static struct BinaryTreeNode *getHard(const struct Token **token, struct SyntaxError *error) {
	if((*token)->type == TOKEN_OPERATOR) {
		const struct Operator *operator = operatorGet((*token)->operator);
		assert(operator);

		if(operator->type != OPERATOR_LBRACKET) {
			error->position = (*token)->position;
			error->type = ERROR_MISSING_BRACKET;
			return NULL;
		}

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
	return getNumber(token, error);
}

static struct BinaryTreeNode *getUnary(const struct Token **token, struct SyntaxError *error) {
	assert(token);
	assert(*token);

	if((*token)->type == TOKEN_OPERATOR) {
		const struct Operator *operator = operatorGet((*token)->operator);
		assert(operator);

		printLog(LOG_VERBOSE, "str=%s type=%d", operator->name, operator->type);
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

	if((*token)->type == TOKEN_OPERATOR)
		printLog(LOG_DEBUG, "deep current %d token %d", deep, operatorGet((*token)->operator)->binaryPriority);

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
	printLog(LOG_DEBUG, "Begin parsing string '%s'", str);

	char *buffer = strdup(str);

	struct Token *tokens = tokensCreate(buffer, error);

	for(const struct Token *token = tokens; token->type != TOKEN_EOF; token++) {
		if(token->type == TOKEN_OPERATOR)
			printLog(LOG_DEBUG, "type = %d, operator=%d", token->type, token->operator);
		else
			printLog(LOG_DEBUG, "type = %d", token->type);
	}

	const struct Token *token = tokens;

	if(!tokens)
		return NULL;

	struct BinaryTreeNode *node = getExpr(&token, error);
	if(!node)
		return NULL;

	if(!token || token->type != TOKEN_EOF)
		return NULL;

	free(tokens);

	return node;
}

