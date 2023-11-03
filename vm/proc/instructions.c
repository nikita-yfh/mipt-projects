#include "processor.h"
#include "instruction.h"
#include "graphics.h"
#include "log.h"

#include <assert.h>
#include <math.h>

enum Direction {
	DIR_IN,
	DIR_OUT
};

static int getArgument(struct Processor *processor,
		struct ProcessorInstruction *instruction, int direction, arg_t **argument) {
	assert(processor);
	assert(instruction);
	assert(argument);

	if(direction == DIR_OUT && (instruction->flags & (FLAG_IMM | FLAG_MEM)) == FLAG_IMM)
		return EXEC_INVALID_FORMAT;

	if(instruction->flags & FLAG_REG)
		if(instruction->reg >= REG_COUNT)
			return EXEC_INVALID_REGISTER;

	if((instruction->flags & (FLAG_REG | FLAG_IMM)) == (FLAG_REG | FLAG_IMM)) { // reg + imm
		static arg_t temp;
		temp = processor->registers[instruction->reg] + instruction->immutable;
		*argument = &temp;
	}
	else if((instruction->flags & (FLAG_REG | FLAG_IMM)) == FLAG_REG) // only reg
		*argument = &processor->registers[instruction->reg];
	else if((instruction->flags & (FLAG_REG | FLAG_IMM)) == FLAG_IMM) // only imm
		*argument = &instruction->immutable;

	if(*argument && (instruction->flags & FLAG_MEM) == FLAG_MEM) {
		if(**argument >= processor->memorySize)
			return EXEC_OUT_OF_BOUNDS;
		*argument = &processor->memory[**argument];
	}

	if(!*argument)
		return EXEC_INVALID_FORMAT;

	return EXEC_OK;
}

typedef int instructionFunction_t(struct Processor*, struct ProcessorInstruction *instruction);

#define STACK_POP(processor, var) \
	do { \
		if(stackPop(&processor->mainStack, var)) \
			return EXEC_STACK_EMPTY; \
	} while(0)

#define STACK_PUSH(processor, var) \
	do { \
		if(stackPush(&processor->mainStack, var)) \
			return EXEC_STACK_OVERFLOW; \
	} while(0)

#define GET_ARGUMENT(processor, instruction, direction, argument) \
	do { \
		int ret = getArgument(processor, instruction, direction, argument); \
		if(ret) \
			return ret; \
		assert(*argument); \
	} while(0)

static int funcNop(struct Processor*, struct ProcessorInstruction*) {
	return EXEC_OK;
}

static int funcHlt(struct Processor *processor, struct ProcessorInstruction*) {
	processor->pc = PC_HLT;
	return EXEC_END_PROGRAM;
}

static int funcPush(struct Processor *processor, struct ProcessorInstruction *instruction) {
	arg_t *arg = NULL;
	GET_ARGUMENT(processor, instruction, DIR_IN, &arg);

	STACK_PUSH(processor, (stackValue_t) *arg);
	return EXEC_OK;
}

static int funcPop(struct Processor *processor, struct ProcessorInstruction *instruction) {
	stackValue_t var = 0;
	STACK_POP(processor, &var);

	arg_t *arg = NULL;
	GET_ARGUMENT(processor, instruction, DIR_OUT, &arg);

	*arg = (arg_t) var;
	return EXEC_OK;
}

static int funcIMath(struct Processor *processor, struct ProcessorInstruction *instruction) {
	stackValue_t a = 0, b = 0, c = 0;
	STACK_POP(processor, &b);
	STACK_POP(processor, &a);

	switch(instruction->command) {
		case C_ADD: c = a + b; break;
		case C_SUB: c = a - b; break;
		case C_MUL: c = a * b; break;
		case C_MOD: c = a % b; break;
		case C_DIV:
			if(b == 0)
				return EXEC_MATH_ERROR;
			c = a / b;
			break;
		default: assert(0);
	}

	STACK_PUSH(processor, c);

	return EXEC_OK;
}

static int funcFMath(struct Processor *processor, struct ProcessorInstruction *instruction) {
	float a = NAN, b = NAN, c = NAN;
	STACK_POP(processor, (stackValue_t*) &b);
	STACK_POP(processor, (stackValue_t*) &a);

	switch(instruction->command) {
		case C_FADD: c = a + b; break;
		case C_FSUB: c = a - b; break;
		case C_FMUL: c = a * b; break;
		case C_FDIV: c = a / b; break;
		default: assert(0);
	}

	STACK_PUSH(processor, *((stackValue_t*) &c));

	return EXEC_OK;
}

static int funcSqrt(struct Processor *processor, struct ProcessorInstruction*) {
	float a = NAN;
	STACK_POP(processor, (stackValue_t*) &a);
	float b = sqrtf(a);
	STACK_PUSH(processor, *((stackValue_t*) &b));
	return EXEC_OK;
}

static int funcIn(struct Processor *processor, struct ProcessorInstruction*) {
	stackValue_t a = 0;
	scanf("%d", &a);
	STACK_PUSH(processor, a);
	return EXEC_OK;
}

static int funcOut(struct Processor *processor, struct ProcessorInstruction*) {
	stackValue_t a = 0;
	STACK_POP(processor, &a);
	printf("%d\n", a);
	return EXEC_OK;
}

static int funcFIn(struct Processor *processor, struct ProcessorInstruction*) {
	float a = NAN;
	scanf("%f", &a);
	STACK_PUSH(processor, *((stackValue_t*) &a));
	return EXEC_OK;
}

static int funcFOut(struct Processor *processor, struct ProcessorInstruction*) {
	float a = 0;
	STACK_POP(processor, (stackValue_t*) &a);
	printf("%g\n", a);
	return EXEC_OK;
}

static int funcDF(struct Processor *processor, struct ProcessorInstruction*) {
	stackValue_t d = 0;
	STACK_POP(processor, &d);
	float f = (float) d;
	STACK_PUSH(processor, *((stackValue_t*) &f));
	return EXEC_OK;
}

static int funcFD(struct Processor *processor, struct ProcessorInstruction*) {
	float f = NAN;
	STACK_POP(processor, (stackValue_t*) &f);
	int d = (int) f;
	STACK_PUSH(processor, d);
	return EXEC_OK;
}

static int funcJump(struct Processor *processor, struct ProcessorInstruction *instruction) {
	arg_t *address = NULL;
	GET_ARGUMENT(processor, instruction, DIR_IN, &address);
	
	processor->pc = *address;
	return EXEC_OK;
}

static int funcJcmp(struct Processor *processor, struct ProcessorInstruction *instruction) {
	arg_t *address = NULL;
	GET_ARGUMENT(processor, instruction, DIR_IN, &address);

	stackValue_t arg = 0;
	STACK_POP(processor, &arg);

	int jump = 1;

	switch(instruction->command) {
		case C_JGE: jump = (arg >= 0); break;
		case C_JG : jump = (arg >  0); break;
		case C_JLE: jump = (arg <= 0); break;
		case C_JL : jump = (arg <  0); break;
		case C_JE:  jump = (arg == 0); break;
		case C_JNE: jump = (arg != 0); break;
		default: assert(0);
	}
	
	if(jump)
		processor->pc = *address;
	return EXEC_OK;
}

static int funcUpd(struct Processor *processor, struct ProcessorInstruction*) {
	if(processor->videoEnable)
		graphicsUpdate();
	return EXEC_OK;
}

static int funcCall(struct Processor *processor, struct ProcessorInstruction *instruction) {
	arg_t *address = NULL;
	GET_ARGUMENT(processor, instruction, DIR_IN, &address);
	
	if(stackPush(&processor->callStack, (stackValue_t) processor->pc))
		return EXEC_STACK_OVERFLOW;
	processor->pc = *address;
	return EXEC_OK;
}

static int funcRet(struct Processor *processor, struct ProcessorInstruction*) {
	int32_t jump = 0;
	if(stackPop(&processor->callStack, &jump))
		return EXEC_STACK_EMPTY;

	processor->pc = (arg_t) jump;
	return EXEC_OK;
}

static instructionFunction_t *functions[] = {
	NULL,
	funcNop,
	funcHlt,
	funcPush,
	funcPop,

	funcIMath,
	funcIMath,
	funcIMath,
	funcIMath,
	funcIMath,
	funcIn,
	funcOut,

	funcFMath,
	funcFMath,
	funcFMath,
	funcFMath,
	funcSqrt,
	funcFIn,
	funcFOut,

	funcDF,
	funcFD,

	funcJump,
	funcJcmp,
	funcJcmp,
	funcJcmp,
	funcJcmp,
	funcJcmp,
	funcJcmp,

	funcCall,
	funcRet,

	funcUpd
};

static_assert(sizeof(functions) / sizeof(*functions) == C_COUNT);

int processorExecNextCommand(struct Processor *processor) {
	if(graphicsUpdateEvents())
		processor->pc = PC_HLT;

	if(processor->pc >= processor->codeSize)
		return EXEC_END_PROGRAM;

	struct ProcessorInstruction *instruction = &processor->code[processor->pc++];

	if(instruction->command == 0 || instruction->command >= C_COUNT)
		return EXEC_INVALID_INSTRUCTION;

	return functions[instruction->command](processor, instruction);
}
