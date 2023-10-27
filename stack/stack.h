#ifndef STACK_H
#define STACK_H

#include <stdint.h>
#include <stddef.h>

#include "config.h"

#ifdef STACK_ENABLE_KAPETZ
typedef uint64_t kapetz_t;
#endif

struct Stack {
#ifdef STACK_ENABLE_KAPETZ
	kapetz_t smallKapetz;
#endif

	size_t size;
	size_t capacity;

	unsigned int structHash;
	unsigned int dataHash;

	int error;

	stackValue_t* values;

#ifdef STACK_ENABLE_KAPETZ
	kapetz_t bigKapetz;
#endif
};

enum STACK_STATE {
	STACK_OK,
	STACK_EMPTY,
	STACK_OVERFLOW,
	STACK_NO_MEMORY,
	STACK_INVALID_DATA,
	STACK_SMALL_KAPETZ,
	STACK_BIG_KAPETZ,
	STACK_INVALID_STRUCT_HASH,
	STACK_INVALID_DATA_HASH,
	STACK_INVALID_POINTER,
	STACK_INVALID_SIZE,
};

int stackCreate(struct Stack* stack);
int stackDelete(struct Stack* stack);

int stackPush(struct Stack* stack, stackValue_t value);
int stackPop(struct Stack* stack, stackValue_t* value);

int stackVerify(struct Stack* stack);
void stackDump(struct Stack* stack, int level);

#endif

