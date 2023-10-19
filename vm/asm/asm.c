#include "asm.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>

#include "instruction.h"
#include "text.h"
#include "utils.h"

struct Label {
	char name[128];
	unsigned int offset;
};

static int checkLabel(const char *buffer, unsigned int *pc, struct Label *label) {
	assert(buffer);
	assert(label);

	if(!*buffer)
		return 0;

	if(*buffer++ != ':') {
		(*pc)++;
		return 0;
	}
	while(*buffer == ' ')
		buffer++;

	strncpy(label->name, buffer, sizeof(label->name));
	strtok(label->name, " ");

	label->offset = *pc;

	return 1;
}

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

	if(sscanf(numberStr, "%d", &number.i) != 1
		&& sscanf(numberStr, "%f", &number.f) != 1)
		error->message = "invalid number value";

	return number.u;
}

static uint32_t readNextLabel(const struct Label *labels, unsigned int labelCount,
									struct AsmError *error) {
	assert(labels);
	assert(error);

	unsigned int offset = UINT_MAX;

	const char *jumpStr = strtok(NULL, " ");
	sscanf(jumpStr, "%u", &offset);

	for(unsigned int label = 0; label < labelCount; label++) {
		printf("%s\n", labels[label].name);
		if(strcmp(labels[label].name, jumpStr) == 0)
			offset = labels[label].offset;
	}

	if(offset == UINT_MAX)
		error->message = "invalid jump label or offset";
	return offset;
}

static int assembleString(const char *buffer, struct ProcessorInstruction *instruction,
							const struct Label *labels, unsigned int labelCount,
							unsigned int *pc, struct AsmError *error) {
	assert(buffer);
	assert(error);
	assert(instruction);
	assert(labels);

	if(!*buffer)
		return 0;

	char line[1024];
	strncpy(line, buffer, sizeof(line));
	stripCommand(line);

	if(*line == ':') {
		const char *label = strtok(line + 1, "");
		while(*label) {
			if(!isalpha(*label)) {
				error->message = "invalid label";
				break;
			}
			label++;
		}
	} else {
		instruction->command = stringToCommand(strtok(line, " "));
		assert(instruction->command < C_COUNT);

		error->arg = 0;

		switch(instruction->command) {
		case C_INVALID:
			error->message = "invalid instruction";
			break;
		case C_PUSH:
			instruction->arg = readNextNumber(error);
			break;
		case C_JMP:
		case C_JA:
		case C_JAE:
		case C_JB:
		case C_JBE:
		case C_JE:
		case C_JNE:
			instruction->arg = readNextLabel(labels, labelCount, error);
			break;
		}

		(*pc)++;
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

int assembleFile(struct AsmInput *input, struct AsmError *error) {
	size_t fileSize = getFileSize(input->in);

	char *file = (char*) calloc(fileSize, sizeof(char*));
	fread(file, fileSize, 1, input->in);

	struct Label *labels = (struct Label*) calloc(countLines(file), sizeof(struct Label));
	cutLines(file);
	unsigned int labelCount = 0;

	unsigned int pc = 0;
	for(size_t offset = 0; offset < fileSize; offset += strlen(file + offset) + 1)
		if(checkLabel(file + offset, &pc, &labels[labelCount]))
			labelCount++;

	unsigned int codeCount = pc;
	struct ProcessorInstruction *code = (struct ProcessorInstruction*)
				calloc(codeCount, sizeof(struct ProcessorInstruction));

	char buffer[4096] = {};
	unsigned int currentLine = 1;
	pc = 0;
	for(size_t offset = 0; offset < fileSize; offset += strlen(file + offset) + 1) {
		error->line = currentLine++;
		if(assembleString(file + offset, &code[pc], labels, labelCount, &pc, error) != 0)
			break;
	}

	if(!error->message)
		fwrite(code, sizeof(struct ProcessorInstruction), codeCount, input->out);
	else
		printError(buffer, error);

	free(file);
	free(code);
	free(labels);

	return error->message ? -1 : 0;
}

