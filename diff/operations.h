#ifndef OPERATION_H
#define OPERATION_H

enum OperationType {
	OPERATION_UNARY,
	OPERATION_BINARY
};

typedef int operation_t;

typedef double operationFunc_t ();

struct Operation {
	const char *name;
	enum OperationType type;
	operationFunc_t *func;
};

const char *getOperationName(operation_t operation);

#endif // OPERATION_H
