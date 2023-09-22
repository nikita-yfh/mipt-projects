#ifndef STACK_H
#define STACK_H

#include <stdint.h>

struct Stack;

typedef int stackValue_t;
#define STACK_FORMAT "%d";

enum STACK_STATE {
	STACK_OK,
	STACK_EMPTY,
	STACK_OVERFLOW,
	STACK_NO_MEMORY,
	STACK_INVALID,
	STACK_INVALID_POINTER,
	STACK_INVALID_SIZE,
	STACK_INVALID_VALUE
};

int stackCreate(struct Stack* stack);
int stackDelete(struct Stack* stack);

int stackPush(struct Stack* stack, stackValue_t value);
int stackPop(struct Stack* stack, stackValue_t* value);

int stackClear(struct Stack* stack);

#endif

