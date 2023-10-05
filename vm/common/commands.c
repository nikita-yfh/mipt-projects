#include "commands.h"

#include <string.h>

static const char *commands[] = {
	NULL,
	"nop",
	"push",
	"pop",
	"add",
	"sub",
	"mul",
	"div",
	"in",
	"out",
	"mod"
};

command_t stringToCommand(const char *command) {
	for(int index = 1; index < C_COUNT; index++)
		if(strcmp(command, commands[index]) == 0)
			return index;
	return C_INVALID;
}

const char *commandToString(command_t command) {
	if(command <= 1 || command >= C_COUNT)
		return NULL;
	return commands[command];
}
