#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h>

typedef uint32_t register_t;

enum {
	REG_INVALID,

	REG_AX,
	REG_BX,
	REG_CX,
	REG_DX,

	REG_COUNT
};

register_t stringToRegister(const char *command);
const char *registerToString(register_t command);

#endif
