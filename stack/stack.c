#include "stack.h"

#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <limits.h>
#include <time.h>

#include "log.h"
#include "murmurhash.h"


// config

#define STACK_RANDOM_KAPETZ
#define STACK_RESIZE_ON_POP
#define STACK_ENABLE_KAPETZ
#define STACK_ENABLE_HASH
#define STACK_MAX_SIZE 1000
#define STACK_DUMP_MAX_VALUES 10
#define STACK_DEFAULT_CAPACITY 8
#define STACK_POISON INT_MAX

#ifdef STACK_ENABLE_KAPETZ
typedef uint64_t kapetz_t;

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


#ifdef STACK_RANDOM_KAPETZ
static uint64_t _getRandomKapetz() {
	static uint64_t kapetz = 0;
	if(kapetz == 0) {
		srand(0);
		union {
			int i[2];
			uint64_t kapetz;
		} kapetzU;
		kapetzU.i[0] = rand(); kapetzU.i[1] = rand();
		kapetz = kapetzU.kapetz;
	}
	return kapetz;
}
#define STACK_KAPETZ_VALUE _getRandomKapetz()
#else
#define STACK_KAPETZ_VALUE 0xEDAEDAEDAEDAEDA8
#endif

#endif

static size_t minSize(size_t a, size_t b) {
	return (a < b) ? a : b;
}

static size_t stackGetValuesSize(size_t capacity) {
	return capacity * sizeof(stackValue_t);
}

static stackValue_t* stackIndex(struct Stack *stack, size_t index) {
	assert(stack);
	return stack->values + index;
}

static int stackSetCapacity(struct Stack* stack) {
	printLog(LOG_INFO, "Set stack capacity to %d", stack->capacity);

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
		printLog(LOG_INFO, "Need to increase capacity");
		stack->capacity = minSize(STACK_MAX_SIZE, stack->capacity * 2);
		return stackSetCapacity(stack);
	}
#ifdef STACK_RESIZE_ON_POP
	else if(stack->size * 2 < stack->capacity &&
			stack->size >= STACK_DEFAULT_CAPACITY * 2) {
		printLog(LOG_INFO, "Need to decrease capacity");
		stack->capacity /= 2;
		return stackSetCapacity(stack);
	}
#endif
	return stack->error;
}

// Hashes

#ifdef STACK_ENABLE_HASH
static unsigned int stackGetStructHash(struct Stack *stack) {
	assert(stack);

	unsigned int lastStructHash = stack->structHash;
	stack->structHash = 0;

	unsigned int lastDataHash   = stack->dataHash;
	stack->dataHash = 0;

	unsigned int hash = murmurHash2(stack, sizeof(struct Stack));

	stack->structHash = lastStructHash;
	stack->dataHash   = lastDataHash;

	return hash;
}

static unsigned int stackGetDataHash(struct Stack *stack) {
	assert(stack);

	return murmurHash2(stack->values, stackGetValuesSize(stack->capacity));
}

static void stackRehash(struct Stack *stack) {
	assert(stack);

	stack->dataHash   = stackGetDataHash(stack);
	stack->structHash = stackGetStructHash(stack);

	printLog(LOG_VERBOSE, "Recalc data hash:   %08X", stack->dataHash);
	printLog(LOG_VERBOSE, "Recalc struct hash: %08X", stack->structHash);
}
#endif


static int stackVerify(struct Stack* stack) {
	if(!stack)
		return STACK_INVALID_POINTER;

	if(stack->size > stack->capacity)
		stack->error = STACK_INVALID_SIZE;
	if(!stack->values)
		stack->error = STACK_INVALID_DATA;
#ifdef STACK_ENABLE_HASH
	if(stack->dataHash != stackGetDataHash(stack))
		stack->error = STACK_INVALID_DATA_HASH;
	if(stack->structHash != stackGetStructHash(stack))
		stack->error = STACK_INVALID_STRUCT_HASH;
#endif
#ifdef STACK_ENABLE_KAPETZ
	if(stack->smallKapetz != STACK_KAPETZ_VALUE)
		stack->error = STACK_SMALL_KAPETZ;
	if(stack->bigKapetz   != STACK_KAPETZ_VALUE)
		stack->error = STACK_BIG_KAPETZ;
#endif
	return stack->error;
}

static const char *stackGetErrorDescription(int error) {
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
	case STACK_SMALL_KAPETZ:
		return "Small stack kapetz";
	case STACK_BIG_KAPETZ:
		return "BIG stack kapetz";
	case STACK_INVALID_STRUCT_HASH:
		return "Struct hash doesn't match";
	case STACK_INVALID_DATA_HASH:
		return "Data hash doesn't match";
	case STACK_INVALID_POINTER:
		return "Invalid pointer";
	case STACK_INVALID_SIZE:
		return "Invalid stack size";
	default:
		return "Unknown error";
	}
}

// Main functions

struct Stack* stackCreate() {
	struct Stack* stack = (struct Stack*) calloc(1, sizeof(struct Stack));

	stack->capacity = STACK_DEFAULT_CAPACITY;

	stack->error = STACK_OK;

	stack->size = 0;
	stackSetCapacity(stack);

#ifdef STACK_ENABLE_KAPETZ
	stack->smallKapetz        = STACK_KAPETZ_VALUE;
	stack->bigKapetz          = STACK_KAPETZ_VALUE;
#endif

#ifdef STACK_ENABLE_HASH
	stackRehash(stack);
#endif

	return stack;
}

int stackDelete(struct Stack* stack) {
	assert(stack);

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

	if(stack->size == STACK_MAX_SIZE) {
		stack->error = STACK_OVERFLOW;
		return stack->error;
	}

	stack->size++;

	ret = stackResize(stack);
	if(ret)
		return ret;

	*stackIndex(stack, stack->size - 1) = value;

	printLog(LOG_INFO, "Stack push "STACK_FORMAT, value);

#ifdef STACK_ENABLE_HASH
	stackRehash(stack);
#endif
	return stack->error;
}

int stackPop(struct Stack* stack, stackValue_t *value) {
	int ret = stackVerify(stack);
	if(ret) return ret;

	if(stack->size == 0) {
		stack->error = STACK_EMPTY;
		return stack->error;
	}

	*value = *stackIndex(stack, --stack->size);

	printLog(LOG_INFO, "Stack pop "STACK_FORMAT, *value);

	ret = stackResize(stack);
#ifdef STACK_ENABLE_HASH
	stackRehash(stack);
#endif
	return ret;
}

void stackDump(struct Stack *stack, int level) {
	printLog(level, "{");
#ifdef STACK_ENABLE_KAPETZ
	printLog(level, "  smallKapetz = %lX,", stack->smallKapetz);
	printLog(level, "  bigKapetz   = %lX,", stack->bigKapetz);
#endif
#ifdef STACK_ENABLE_HASH
	printLog(level, "  dataHash    = %08X,",stack->dataHash);
	printLog(level, "  structHash  = %08X,",stack->structHash);
#endif
	printLog(level, "  size        = %lu,", stack->size);
	printLog(level, "  capacity    = %lu,", stack->capacity);
	printLog(level, "  error       = %d [%s],",
			stack->error, stackGetErrorDescription(stack->error));
	printLog(level, "  values [%p] = {",    stack->values);

	if(stack->values) {
		for(size_t index = 0; index < minSize(STACK_DUMP_MAX_VALUES, stack->size); index++) {
			stackValue_t *value = stackIndex(stack, index);

			printLog(level, "    [%lu] [%p] = "STACK_FORMAT",",
					index, value, *value);
		}
		if(stack->size > STACK_DUMP_MAX_VALUES)
			printLog(level, "    ............................[%d]",
					stack->size - STACK_DUMP_MAX_VALUES);
	}
	printLog(level, "  }");
	printLog(level, "}");
}
