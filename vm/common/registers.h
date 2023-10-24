#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h>

typedef uint8_t reg_t;

enum {
	REG_INVALID,

	REG_AX,
	REG_BX,
	REG_CX,
	REG_DX,

	REG_COUNT
};

reg_t stringToRegister(const char *command);
const char *registerToString(reg_t command);

#endif
