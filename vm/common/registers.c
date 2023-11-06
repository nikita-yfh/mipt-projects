#include "registers.h"

#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "utils.h"

reg_t stringToRegister(const char *reg) {
	assert(reg);
	if(strlen(reg) != 3)
		return REG_INVALID;

	int letter = tolower(reg[1]);
	if(tolower(reg[0]) != 'r' || letter < 'a' || letter >= 'a' + REG_COUNT || reg[2] != 'x')
		return REG_INVALID;
	return (reg_t) (letter - 'a');
}

const char *registerToString(reg_t reg) {
	if(reg >= REG_COUNT)
		return NULL;

	static char str[] = "r-x";
	str[1] = (char) ('a' + reg);

	return str;
}
