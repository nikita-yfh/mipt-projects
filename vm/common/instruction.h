#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>
#include <stdbool.h>

#include "registers.h"

typedef uint8_t command_t;
typedef uint8_t flags_t;
typedef uint32_t arg_t;

#define C_INVALID 0x00

enum FLAGS {
	FLAG_IMM = (1 << 0),
	FLAG_REG = (1 << 1),
	FLAG_MEM = (1 << 2)
};

struct ProcessorInstruction {
	uint8_t __pad;
	reg_t reg;
	flags_t flags;
	command_t command;
	arg_t immutable;
};

// TODO: this could be a const size_t instead
#define MAX_CODE_SIZE (sizeof(struct ProcessorInstruction) * UINT32_MAX)

command_t stringToCommand(const char *command);
const char *commandToString(command_t command);

bool needArgument(command_t command);

#endif
