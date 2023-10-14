#include "asm.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "instruction.h"
#include "text.h"
#include "utils.h"


static void stripCommand(char *str) {
	assert(str);

	char *newline = strchr(str, '\n');
	if(newline)
		*newline = '\0';

	char *begin = strchr(str, ';');
	if(begin)
		*begin = '\0';
}

static uint32_t readNextNumber(struct AsmError *error) {
	assert(error);
	error->arg++;

	const char *numberStr = strtok(NULL, " ");

	if(!numberStr) {
		error->message = "to few arguments";
		return 0;
	}
	
	union {
		int i;
		uint32_t u;
		float f;
	} number;

	if(sscanf(numberStr, "%d", &number.i) != 1)
		error->message = "invalid number value";

	return number.u;
}

static int assembleString(const char *buffer, struct ProcessorInstruction *instruction,
										struct AsmError *error) {
	assert(buffer);
	assert(error);

	char str[1024];
	strncpy(str, buffer, sizeof(str));
	stripCommand(str);

	const char *command = strtok(str, " ");
	instruction->command = stringToCommand(command);
	assert(instruction->command < C_COUNT);

	error->arg = 0;

	switch(instruction->command) {
	case C_INVALID:
		error->message = "invalid instruction";
		break;
	case C_PUSH:
		instruction->arg = readNextNumber(error);
		break;
	}

	if(!error->message) {
		error->arg++;
		if(strtok(NULL, " "))
			error->message = "too much arguments";
	}

	return error->message ? -1 : 0;
}

static void printError(const char *str, const struct AsmError *error) {
	fprintf(stderr, "%s:%u: \e[1;31mError:\e[0m %s\n",
			error->file, error->line, error->message);

	unsigned int arg = 0;
	int highlight = 0;

	while(*str) {
		if(highlight == 0 && arg == error->arg) {
			fputs("\e[1;35m", stderr);
			highlight = 1;
		}

		if(*str == ' ') {
			if(highlight) {
				fputs("\e[0m", stderr);
				highlight = 0;
			}
			arg++;
		}
		fputc(*str++, stderr);
	}
	fputs("\e[0m", stderr);
}


int assembleFile(FILE *in, FILE *out, struct AsmError *error) {
	char buffer[1024];
	unsigned int lineNumber = 0;

	while(fgets(buffer, sizeof(buffer), in) != NULL) {
		error->line = ++lineNumber;

		struct ProcessorInstruction instruction = {};
		if(assembleString(buffer, &instruction, error) != 0) {
			printError(buffer, error);
			return -1;
		}

		fwrite(&instruction, sizeof(instruction), 1, out);
	}
	return 0;
}

