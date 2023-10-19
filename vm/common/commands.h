#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>

typedef uint8_t command_t;

enum {
	C_INVALID,

	C_NOP,
	C_HLT,
	C_PUSH,
	C_PUSH_REG,
	C_POP_REG,
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

command_t stringToCommand(const char *command);
const char *commandToString(command_t command);

#endif
