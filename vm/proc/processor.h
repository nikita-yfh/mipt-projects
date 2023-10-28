#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <stdio.h>

#include "instruction.h"
#include "registers.h"

#include "stack.h"

#define PC_HLT UINT32_MAX; 

enum ExecCommandStatus {
	EXEC_ERR = -1,
	EXEC_OK  =  0,
	EXEC_END =  1
};

struct ProcessorInput {
	FILE *file;
	const char *fileName;

	arg_t memorySize;

	int skipHeader;
	int ignoreHeader;
	uint8_t defaultMemValue;

	arg_t initCodeOffset;
};

struct Processor {
	struct Stack mainStack;
	struct Stack callStack;

	arg_t pc; 
	struct ProcessorInstruction *code;
	arg_t codeSize;

	arg_t registers[REG_COUNT];

	arg_t *memory;
	arg_t memorySize;
};

int processorCreate(struct Processor *processor, const struct ProcessorInput *input);
int processorDelete(struct Processor *processor);

int processorVerify(struct Processor *processor);
void processorDump(struct Processor *processor);

int processorExecNextCommand(struct Processor *processor);

#endif
