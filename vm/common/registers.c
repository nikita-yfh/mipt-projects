#include "registers.h"

#include <string.h>
#include <assert.h>

#include "utils.h"

static const char *registers[] = {
	"rax",
	"rbx",
	"rcx",
	"rdx"
};

static_assert(sizeof(registers) / sizeof(const char*) == REG_COUNT);

reg_t stringToRegister(const char *reg) {
	assert(reg);

	for(reg_t index = 0; index < REG_COUNT; index++)
		if(stricmp(reg, registers[index]) == 0)
			return index;
	return REG_INVALID;
}

const char *registerToString(reg_t reg) {
	if(reg >= REG_COUNT)
		return NULL;
	return registers[reg];
}
