#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <stdio.h>

#include "instruction.h"
#include "registers.h"

#include "stack.h"

#define PC_HLT UINT32_MAX; 

enum ExecCommandStatus {
	EXEC_INVALID_FORMAT        = -7,
	EXEC_INVALID_INSTRUCTION   = -6,
	EXEC_INVALID_REGISTER      = -5,
	EXEC_MATH_ERROR            = -4,
	EXEC_STACK_OVERFLOW        = -3,
	EXEC_STACK_EMPTY           = -2,
	EXEC_OUT_OF_BOUNDS         = -1,
	EXEC_OK                    =  0,
	EXEC_END_PROGRAM           =  1
};

struct ProcessorInput {
	FILE *file;
	const char *fileName;

	arg_t memorySize;

	int skipHeader;
	int ignoreHeader;
	int verboseLog;
	uint8_t defaultMemValue;

        // TODO: maybe video parameters can isolated?
	int videoEnable;
	arg_t videoWidth;
	arg_t videoHeight;
	arg_t videoOffset;

	arg_t initCodeOffset;
};

struct Processor {
	struct Stack mainStack;
	struct Stack callStack;

	arg_t pc; 
	struct ProcessorInstruction *code; // TODO: again, this is an array, extract (you can make macro-based generic array)
	arg_t codeSize;

	arg_t registers[REG_COUNT];

	arg_t *memory; // TODO: and again, an array
	arg_t memorySize;

	int verboseLog;
	int videoEnable;
};

int processorCreate(struct Processor *processor, const struct ProcessorInput *input);
int processorDelete(struct Processor *processor);

int processorVerify(struct Processor *processor);
void processorDump(struct Processor *processor, int level);

int processorRun(struct Processor *processor);
int processorExecNextCommand(struct Processor *processor);

#endif
