#include "registers.h"

#include <string.h>
#include <assert.h>

#include "utils.h"

static const char *registers[] = {
	NULL,

	"rax",
	"rbx",
	"rcx",
	"rdx"
};

static_assert(sizeof(registers) / sizeof(const char*) == REG_COUNT);

register_t stringToRegister(const char *reg) {
	assert(reg);

	for(register_t index = 1; index < REG_COUNT; index++)
		if(stricmp(reg, registers[index]) == 0)
			return index;
	return REG_INVALID;
}

const char *registerToString(register_t reg) {
	if(reg <= 1 || reg >= REG_COUNT)
		return NULL;
	return registers[reg];
}
