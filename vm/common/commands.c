#include "commands.h"

#include <string.h>
#include <assert.h>

#include "utils.h"

static const char *commands[] = {
	NULL,
	"nop",
	"hlt",
	"push",
	"push",
	"pop",
	"add",
	"sub",
	"mul",
	"div",
	"in",
	"out",
	"mod",
	"jmp",
	"ja",
	"jae",
	"jb",
	"jbe",
	"je",
	"jne"
};

static_assert(sizeof(commands) / sizeof(const char*) == C_COUNT);

command_t stringToCommand(const char *command) {
	assert(command);

	for(command_t index = 1; index < C_COUNT; index++)
		if(stricmp(command, commands[index]) == 0)
			return index;
	return C_INVALID;
}

const char *commandToString(command_t command) {
	if(command <= 1 || command >= C_COUNT)
		return NULL;
	return commands[command];
}
