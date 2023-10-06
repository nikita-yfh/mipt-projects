#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "commands.h"

struct ProcessorInstruction {
	command_t command;
	uint8_t bitMap;
	uint32_t arg1;
	uint32_t arg2;
};

#endif
