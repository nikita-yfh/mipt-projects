#include "stack.h"

#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <limits.h>

#define STACK_RESIZE_ON_POP
#define STACK_ENABLE_KAPETZ
#define STACK_ENABLE_HASH
#define STACK_MAX_SIZE SIZE_MAX
#define STACK_DUMP_MAX_VALUES 10
#define STACK_DEFAULT_CAPACITY 8
#define STACK_POISON INT_MAX

#ifdef STACK_ENABLE_KAPETZ
typedef uint64_t kapetz_t;
#define STACK_KAPETZ_VALUE 0xEDAEDAEDAEDAEDA8
#endif

static size_t minSize(size_t a, size_t b) {
	return (a < b) ? a : b;
}

struct Stack {
#ifdef STACK_ENABLE_KAPETZ
	kapetz_t smallKapetz;
#endif

	size_t size;
	size_t capacity;

	int error;

	stackValue_t* values;

#ifdef STACK_ENABLE_KAPETZ
	kapetz_t bigKapetz;
#endif
};

static size_t stackGetValuesSize(size_t capacity) {
#ifdef STACK_ENABLE_KAPETZ
	return sizeof(kapetz_t) * 2 + capacity * sizeof(stackValue_t);
#else
	return capacity * sizeof(stackValue_t);
#endif
}

static stackValue_t* stackIndex(struct Stack *stack, size_t index) {
	assert(stack);
#ifdef STACK_ENABLE_KAPETZ
	return (stackValue_t*)((char*)stack->values + sizeof(kapetz_t) * 2 + index * sizeof(stackValue_t));
#else
	return stack->values + index;
#endif
}

static int stackSetCapacity(struct Stack* stack) {
	if(!stack->values)
		stack->values = calloc(1, stackGetValuesSize(stack->capacity));
	else {
		stack->values = realloc(stack->values, stackGetValuesSize(stack->capacity));
		for(size_t i = stack->size; i < stack->capacity; i++)
			*stackIndex(stack, i) = STACK_POISON;
	}
	if(!stack->values)
		stack->error = STACK_NO_MEMORY;
	return stack->error;
}

static int stackResize(struct Stack* stack) {
	if(stack->size > stack->capacity) {
		stack->capacity = minSize(STACK_MAX_SIZE, stack->capacity * 2);
		return stackSetCapacity(stack);
	}
#ifdef STACK_RESIZE_ON_POP
	else if(stack->size < stack->capacity * 2 &&
			stack->size >= STACK_DEFAULT_CAPACITY * 2) {
		stack->capacity /= 2;
		return stackSetCapacity(stack);
	}
#endif
#ifdef STACK_ENABLE_KAPETZ
	*((kapetz_t*) stack->values) = STACK_KAPETZ_VALUE;
	*((kapetz_t*) stack->values +  stack->capacity * sizeof(stackValue_t) +
	           sizeof(kapetz_t)) = STACK_KAPETZ_VALUE;
#endif
	return stack->error;
}

static int stackVerify(struct Stack* stack) {
	if(!stack)
		return STACK_INVALID_POINTER;

	if(stack->size > stack->capacity)
		stack->error = STACK_INVALID_SIZE;
	if(!stack->values)
		stack->error = STACK_INVALID_DATA;
#ifdef STACK_ENABLE_KAPETZ
	if(stack->smallKapetz != STACK_KAPETZ_VALUE)
		stack->error = STACK_SMALL_KAPETZ;
	if(stack->bigKapetz   != STACK_KAPETZ_VALUE)
		stack->error = STACK_BIG_KAPETZ;

	if(*((kapetz_t*)stack->values) != STACK_KAPETZ_VALUE)
		stack->error = STACK_DATA_SMALL_KAPETZ;
	if(*((kapetz_t*)stack->values + sizeof(kapetz_t) + stack->capacity * sizeof(stackValue_t))
	                               != STACK_KAPETZ_VALUE)
		stack->error = STACK_DATA_BIG_KAPETZ;
#endif
	return stack->error;
}

struct Stack* stackCreate() {
	struct Stack* stack = (struct Stack*) calloc(1, sizeof(struct Stack));

	stack->capacity = STACK_DEFAULT_CAPACITY;

	stack->size = 0;
	stackSetCapacity(stack);

#ifdef STACK_ENABLE_KAPETZ
	stack->smallKapetz        = STACK_KAPETZ_VALUE;
	stack->bigKapetz          = STACK_KAPETZ_VALUE;
#endif

	return stack;
}

int stackDelete(struct Stack* stack) {
	int ret = stackVerify(stack);
	if(ret) return ret;

	free(stack->values);
	stack->values = NULL;

#ifdef STACK_ENABLE_KAPETZ
	stack->smallKapetz = 0;
	stack->bigKapetz   = 0;
#endif

	free(stack);

	return STACK_OK;
}

int stackPush(struct Stack* stack, stackValue_t value) {
	int ret = stackVerify(stack);
	if(ret) return ret;

	if(stack->size == STACK_MAX_SIZE)
		return STACK_OVERFLOW;

	stack->size++;

	ret = stackResize(stack);
	if(ret)
		return ret;

	*stackIndex(stack, stack->size - 1) = value;

	return stack->error;
}

int stackPop(struct Stack* stack, stackValue_t *value) {
	int ret = stackVerify(stack);
	if(ret) return ret;

	if(stack->size == 0)
		return STACK_EMPTY;

	*value = *stackIndex(stack, --stack->size);

	return stackResize(stack);
}

const char *stackGetErrorDescription(int error) {
	switch(error) {
	case STACK_OK:
		return "OK";
	case STACK_EMPTY:
		return "Stack is empty";
	case STACK_OVERFLOW:
		return "Stack overflow";
	case STACK_NO_MEMORY:
		return "No enough memory";
	case STACK_INVALID_DATA:
		return "Invalid data pointer";
	case STACK_INVALID_POINTER:
		return "Invalid pointer";
	case STACK_INVALID_SIZE:
		return "Invalid stack size";
	default:
		return "Unknown error";
	}
}

void stackDump(struct Stack *stack, FILE *file) {
	fprintf(file,
		"{\n"
#ifdef STACK_ENABLE_KAPETZ
		"  smallKapetz = %lX,\n"
		"  bigKapetz   = %lX,\n"
#endif
		"  size        = %lu,\n"
		"  capacity    = %lu,\n"
		"  error       = %d [%s],\n"
		"  values [%p] = {\n",
#ifdef STACK_ENABLE_KAPETZ
	stack->smallKapetz, stack->bigKapetz,
#endif
	stack->size, stack->capacity,
	stack->error, stackGetErrorDescription(stack->error), stack->values);
	if(stack->values) {
		for(size_t index = 0; index < minSize(STACK_DUMP_MAX_VALUES, stack->size); index++) {
			stackValue_t *value = stackIndex(stack, index);

			fprintf(file, "    [%lu] [%p] = "STACK_FORMAT",\n",
					index, value, *value);
		}
	}
	fprintf(file,
		"  }\n"
		"}\n");
}
