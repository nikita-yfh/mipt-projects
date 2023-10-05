#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>

typedef uint8_t command_t;

enum {
	C_INVALID  = 0x00,
	C_NOP      = 0x01,
	C_PUSH     = 0x02,
	C_POP      = 0x03,
	C_ADD      = 0x04,
	C_SUB      = 0x05,
	C_MUL      = 0x06,
	C_DIV      = 0x07,
	C_IN       = 0x08,
	C_OUT      = 0x09,
	C_MOD      = 0x0A,
	C_COUNT
};

command_t stringToCommand(const char *command);
const char *commandToString(command_t command);

#endif
