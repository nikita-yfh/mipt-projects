#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <limits.h>
#include <stddef.h>

enum OperatorType {
	OPERATOR_NONE      = 0,
	OPERATOR_LBRACKET  = 1,
	OPERATOR_RBRACKET  = 2,

	OPERATOR_UNARY     = 4,
	OPERATOR_BINARY    = 8,
	OPERATOR_UBINARY   = OPERATOR_UNARY | OPERATOR_BINARY,
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

#define OPERATOR_MAX_PRIORITY 4

operator_t operatorFindL(const char *text, size_t textlength);
operator_t operatorFind (const char *text);

const struct Operator *operatorGet  (operator_t number);

#endif // OPERATIONS_H
