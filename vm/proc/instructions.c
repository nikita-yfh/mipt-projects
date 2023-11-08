#include "processor.h"
#include "instruction.h"
#include "graphics.h"
#include "log.h"

#include <assert.h>
#include <stdbool.h>
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

#define STACK_POP(processor, var)											\
	do {																	\
		if(stackPop(&processor->mainStack, var))							\
			return EXEC_STACK_EMPTY;										\
	} while(0)

#define STACK_PUSH(processor, var)											\
	do {																	\
		if(stackPush(&processor->mainStack, var))							\
			return EXEC_STACK_OVERFLOW;										\
	} while(0)

// TODO: do not use names like "ret", it lazy and utterly meaningless
//       also, I think pattern if (error) return error; can be extracted.
#define GET_ARGUMENT(processor, instruction, direction, argument)			\
	do {																	\
		int ret = getArgument(processor, instruction, direction, argument);	\
		if(ret)																\
			return ret;														\
		assert(*argument);													\
	} while(0)

int processorExecNextCommand(struct Processor *processor) {
	if(graphicsUpdateEvents())
		processor->pc = PC_HLT;

	if(processor->pc >= processor->codeSize)
		return EXEC_END_PROGRAM;

	struct ProcessorInstruction *instruction = &processor->code[processor->pc++];

	switch(instruction->command) {

#define DEF_COMMAND(name, number, arg, code) \
	case number: code break;
#include "commands.gen.h"
#undef DEF_COMMAND

		default: return EXEC_INVALID_INSTRUCTION;
	}
	return EXEC_INVALID_INSTRUCTION;
}
