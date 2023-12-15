#include "parser.h"

#include <assert.h>
#include <string.h>

static struct BinaryTreeNode *getBinary(const struct Token **token, struct SyntaxError *error);
static struct BinaryTreeNode *getUnary(const struct Token **token, struct SyntaxError *error);

static struct BinaryTreeNode *getNumber(const struct Token **token, struct SyntaxError *error) {
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
		struct BinaryTreeNode *node = getBinary(token, error);

		if(!node)
			return NULL;

		(*token)++;
		operator = operatorGet((*token)->operator);

		return node;
	}
	return getNumber(token, error);
}

static struct BinaryTreeNode *getUnary(const struct Token **token, struct SyntaxError *error) {
	assert(token);
	assert(*token);

	if((*token)->type == TOKEN_OPERATOR) {
		const struct Operator *operator = operatorGet((*token)->operator);
		assert(operator);

		if(operator->type | OPERATOR_UNARY) {
			struct BinaryTreeNode *node = btreeNewOperator((*token)->operator);
			(*token)++;
			node->left = getUnary(token, error);
			return node;
		} else {
			error->position = (*token)->position;
			error->type = ERROR_NOT_UNARY;
			return NULL;
		}
	}

	return getHard(token, error);
}

static struct BinaryTreeNode *getBinary(const struct Token **token, struct SyntaxError *error) {
}

struct BinaryTreeNode *parseString(const char *str, struct SyntaxError *error) {
	char *buffer = strdup(str);

}
