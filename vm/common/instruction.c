#include "instruction.h"

#include <string.h>
#include <assert.h>

#include "utils.h"

static const char *commands[] = { // TODO: read my commands on such arrays from proc
	NULL,

	"nop",
	"hlt",
	"push",
	"pop",

	"add",
	"sub",
	"mul",
	"div",
	"mod",
	"in",
	"out",

	"fadd",
	"fsub",
	"fmul",
	"fdiv",
	"sqrt",
	"fin",
	"fout",

	"df",
	"fd",

	"jmp",
	"jg",
	"jge",
	"jl",
	"jle",
	"je",
	"jne",

	"call",
	"ret",
	"upd"
};

static_assert(sizeof(struct ProcessorInstruction) == 8);
static_assert(sizeof(commands) / sizeof(const char*) == C_COUNT);

// TODO: also, is there a way to extract commands from names?
command_t stringToCommand(const char *command) {
	assert(command);

	for(command_t index = 1; index < C_COUNT; index++)
		if(stricmp(command, commands[index]) == 0)
			return index;
	return C_INVALID;
}

// TODO: and the same thing vice versa
const char *commandToString(command_t command) {
	if(command <= 1 || command >= C_COUNT)
		return NULL;
	return commands[command];
}
