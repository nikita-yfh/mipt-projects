#ifndef OPERATION_H
#define OPERATION_H

enum OperationType {
	OPERATION_UNARY,
	OPERATION_BINARY
};

typedef double operationFunc_t ();

struct Operation {
	const char *str;
	enum OperationType type;
	operationFunc_t *func;
};

#endif // OPERATION_H
