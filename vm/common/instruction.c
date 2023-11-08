#include "instruction.h"

#include <string.h>
#include <assert.h>

#include "utils.h"

command_t stringToCommand(const char *command) {
	assert(command);

#define DEF_COMMAND(name, number, arg, code)	\
	if(stricmp(command, name) == 0)				\
		return number;
#include "commands.gen.h"
#undef DEF_COMMAND

	return C_INVALID;
}

const char *commandToString(command_t command) {
	switch(command) {

#define DEF_COMMAND(name, number, arg, code)	\
	case number: return name;
#include "commands.gen.h"
#undef DEF_COMMAND

	default: return NULL;
	}
}

bool needArgument(command_t command) {
	switch(command) {

#define DEF_COMMAND(name, number, arg, code)	\
	case number: return arg;
#include "commands.gen.h"
#undef DEF_COMMAND

	default: return NULL; // TODO: it's a bool-returning function, why NULL?
	}
}
