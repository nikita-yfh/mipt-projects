#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "commands.h"

struct ProcessorInstruction {
	command_t command;
	uint32_t arg;
};

#endif
