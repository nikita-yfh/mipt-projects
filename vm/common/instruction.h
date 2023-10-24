#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>

#include "registers.h"

typedef uint8_t command_t;
typedef uint8_t flags_t;
typedef uint32_t arg_t;

enum FLAGS {
	FLAG_IMM = (1 << 0),
	FLAG_REG = (1 << 1),
	FLAG_MEM = (1 << 2)
};

enum COMMANGS {
	C_INVALID,

	C_NOP,
	C_HLT,
	C_PUSH,
	C_POP,
	C_ADD,
	C_SUB,
	C_MUL,
	C_DIV,
	C_IN,
	C_OUT,
	C_MOD,
	C_JMP,
	C_JA,  // >
	C_JAE, // >=
	C_JB,  // <
	C_JBE, // <=
	C_JE,  // ==
	C_JNE, // !=

	C_COUNT
};

struct ProcessorInstruction {
	uint8_t __pad;
	reg_t reg;
	flags_t flags;
	command_t command;
	arg_t immutable;
};

command_t stringToCommand(const char *command);
const char *commandToString(command_t command);

#endif
