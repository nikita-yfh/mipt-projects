#ifndef STACK_H
#define STACK_H

#include <stdint.h>

struct Stack;

typedef int stackValue_t;
#define STACK_FORMAT "%d"

enum STACK_STATE {
	STACK_OK,
	STACK_EMPTY,
	STACK_OVERFLOW,
	STACK_NO_MEMORY,
	STACK_INVALID_DATA,
	STACK_SMALL_KAPETZ,
	STACK_BIG_KAPETZ,
	STACK_DATA_SMALL_KAPETZ,
	STACK_DATA_BIG_KAPETZ,
	STACK_INVALID_POINTER,
	STACK_INVALID_SIZE,
	STACK_INVALID_VALUE
};

struct Stack *stackCreate();
int stackDelete(struct Stack* stack);

int stackPush(struct Stack* stack, stackValue_t value);
int stackPop(struct Stack* stack, stackValue_t* value);

void stackDump(struct Stack* stack);

const char *stackGetErrorDescription(int error);

#endif

