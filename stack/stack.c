#include "stack.h"

#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>

const uint32_t STACK_INIT_MAGIC = 0xDED32DED;
const uint32_t STACK_CAPICITY_INC = 10;

#define STACK_RESIZE_ON_POP

static size_t minSize(size_t a, size_t b) {
	return (a > b) ? a : b;
}

struct Stack {
	uint32_t initialized;

	size_t size;
	size_t capacity;

	int locked;

	stackValue_t* values;
};

static int stackSetCapacity(struct Stack* stack) {
	if(!stack->values)
		stack->values = calloc(stack->capacity, sizeof(stackValue_t));
	else
		stack->values = realloc(stack->values, stack->capacity * sizeof(stackValue_t));
	if(!stack->values)
		return STACK_NO_MEMORY;

	return 0;
}

static int stackResize(struct Stack* stack) {
	if(stack->size > stack->capacity) {
		stack->capacity = minSize(stack->size + STACK_CAPICITY_INC, STACK_CAPICITY_INC);
		return stackSetCapacity(stack);
	}
#ifndef STACK_RESIZE_ON_POP
	else if(stack->size + STACK_CAPICITY_INC < stack->capacity) {
		stack->capacity = stack->size;
		return stackSetCapacity(stack);
	}
#endif
	return STACK_OK;
}

static void stackLock(struct Stack* stack) {
	while(stack->locked);
	stack->locked = 1;
}

static void stackUnlock(struct Stack* stack) {
	stack->locked = 0;
}

static int stackLockCheck(struct Stack* stack) {
	if(!stack)
		return STACK_INVALID_POINTER;

	stackLock(stack);

	if(stack->initialized != STACK_INIT_MAGIC)
		return STACK_INVALID;
	if(stack->size > stack->capacity)
		return STACK_INVALID_SIZE;
	return STACK_OK;
}

int stackCreate(struct Stack* stack) {
	int ret = stackLockCheck(stack);
	if(ret) return ret;

	stack->initialized = STACK_INIT_MAGIC;
	stack->capacity = STACK_CAPICITY_INC;
	stack->size = 0;

	stackUnlock(stack);

	return STACK_OK;
}

int stackDelete(struct Stack* stack) {
	int ret = stackLockCheck(stack);
	if(ret) return ret;

	free(stack->values);
	stack->values = NULL;
	stack->initialized = 0;

	return STACK_OK;
}

int stackPush(struct Stack* stack, stackValue_t value) {
	int ret = stackLockCheck(stack);
	if(ret) return ret;

	if(stack->size == SIZE_MAX)
		return STACK_OVERFLOW;

	stack->size++;

	ret = stackResize(stack);
	if(ret) return ret;

	stack->values[stack->size - 1] = value;

	stackUnlock(stack);

	return 0;
}

int stackPop(struct Stack* stack, stackValue_t *value) {
	int ret = stackLockCheck(stack);
	if(ret) return ret;

	if(stack->size == 0)
		return STACK_EMPTY;

	*value = stack->values[stack->size - 1];

	stack->size--;

	ret = stackResize(stack);
	if(ret) return ret;


	stackUnlock(stack);

	return 0;
}

int stackClear(struct Stack* stack) {
	int ret = stackLockCheck(stack);
	if(ret) return ret;

	stack->size = 0;
	stackResize(stack);

	stackUnlock(stack);

	return STACK_OK;
}
