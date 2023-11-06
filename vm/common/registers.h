#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h>

typedef uint8_t reg_t;

enum {
	REG_COUNT = 4,
	REG_INVALID = 0xFF
};

reg_t stringToRegister(const char *command);
const char *registerToString(reg_t command);

#endif
