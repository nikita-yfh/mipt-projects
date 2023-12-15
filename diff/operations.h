#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <limits.h>
#include <stddef.h>

enum OperatorType {
	OPERATOR_NONE,
	OPERATOR_UNARY,
	OPERATOR_BINARY,
	OPERATOR_UBINARY,
	
	OPERATOR_LBRACKET,
	OPERATOR_RBRACKET,
};

typedef double binaryFunc_t(double, double);
typedef double unaryFunc_t(double);

struct Operator {
	const char *name;
	int type;
	int binaryPriority;

	binaryFunc_t *binaryFunc;
	unaryFunc_t  *unaryFunc;
};

typedef unsigned int operator_t;
#define OPERATOR_INVALID UINT_MAX

operator_t operatorFindL(const char *text, size_t textlength);
operator_t operatorFind (const char *text);

const struct Operator *operatorGet  (operator_t number);

#endif // OPERATIONS_H
