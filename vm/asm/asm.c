#include "asm.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>

#include "registers.h"
#include "instruction.h"
#include "colors.h"
#include "utils.h"
#include "header.h"

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

static int readNumber(const char *str, arg_t *value, struct AsmError *error) {
	assert(error);
	assert(str);
	assert(value);

	if(sscanf(str, "%d", (int*)   &value) != 1 &&
	   sscanf(str, "%f", (float*) &value) != 1) {
		error->message = "invalid number value";
		return -1;
	}
	return 0;
}

static int readLabel(const char *str, const struct Label *labels,
		unsigned int labelCount, arg_t *value, struct AsmError *error) {

	assert(labels);
	assert(error);
	assert(value);
	assert(str);

	assert(*str == ':');
	str++; // skip :

	for(unsigned int label = 0; label < labelCount; label++) {
		if(strcmp(labels[label].name, str) == 0) {
			*value = labels[label].offset;
			return 0;
		}
	}

	error->message = "invalid label or offset";
	return -1;
}

static int readRegister(const char *str, reg_t *reg, struct AsmError *error) {
	assert(error);
	assert(str);
	assert(reg);

	*reg = stringToRegister(str);

	if(*reg == REG_INVALID) {
		error->message = "invalid register name"; 
		return -1;
	}
	return 0;
}

static int readArgument(const char *argument, struct ProcessorInstruction *instruction,
		const struct Label *labels, unsigned int labelCount, struct AsmError *error) {
	if(!argument)
		return 0;

	assert(instruction);
	assert(labels);
	assert(error);

	if(*argument == ':') { // label

		if(instruction->flags & FLAG_IMM)
			error->message = "instruction already has immutable const";
		else
			return readLabel(argument, labels, labelCount, &instruction->immutable, error);
		return -1;

	} else if(isdigit(*argument)) { // immutable const

		if(instruction->flags & FLAG_IMM)
			error->message = "instruction already has immutable const";
		else
			return readNumber(argument, &instruction->immutable, error);
		return -1;

	} else if(isalpha(*argument)) { // register

		if(instruction->flags & FLAG_REG)
			error->message = "instruction already has register";
		else
			return readRegister(argument, &instruction->reg, error);
		return -1;

	}

	error->message = "invalid argument";
	return -1;
}

static int readMemoryAccess(const char *line, struct ProcessorInstruction *instruction,
														struct AsmError *error) {
	const char *beginBracket = strchr(line, '[');
	const char *endBracket   = strchr(line, ']');

	if(!beginBracket && !endBracket)
		return 0;

	if(!!beginBracket ^ !!endBracket) {
		error->message = "mismatched bracket";
		if(beginBracket)
			error->offset = (unsigned int)(beginBracket - line);
		else
			error->offset = (unsigned int)(endBracket   - line);
		return -1;
	}

	const char *nextBeginBracket = strchr(beginBracket, '[');
	const char *nextEndBracket   = strchr(endBracket,   ']');

	if(!nextBeginBracket && !nextEndBracket) {
		instruction->flags |= FLAG_MEM;
		return 0;
	}

	error->message = "mismatched bracket";

	if(nextBeginBracket)
		error->offset = (unsigned int)(nextBeginBracket - line);
	if(nextEndBracket)
		error->offset = (unsigned int)(nextEndBracket   - line);

	return -1;
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
		const char *div = " []+";

		if(readMemoryAccess(line, instruction, error))
			return -1;

		const char *commandString = strtok(line, div);

		instruction->command = stringToCommand(commandString);
		assert(instruction->command < C_COUNT);

		if(instruction->command == C_INVALID) {
			error->offset = (unsigned int)(commandString - line);
			error->message = "invalid command";
		}

		const char *arg1 = strtok(NULL, div);
		const char *arg2 = strtok(NULL, div);

		if(readArgument(arg1, instruction, labels, labelCount, error)) {
			error->offset = (unsigned int)(arg1 - line);
			return -1;
		}
		if(readArgument(arg2, instruction, labels, labelCount, error)) {
			error->offset = (unsigned int)(arg2 - line);
			return -1;
		}

		(*pc)++;
	}

	return error->message ? -1 : 0;
}

static void printError(const char *str, const struct AsmError *error) {
	fprintf(stderr, "%s:%u: " COLOR_RED "Error:" COLOR_NONE " %s\n",
			error->file, error->line, error->message);

	unsigned int arg = 0;
	int highlight = 0;

	fprintf(stderr, "%4u |   ", error->line);

	unsigned int wordLength = 0;

	unsigned int counter = 0;
	while(*str) {
		if(highlight == 0 && counter == error->offset) {
			fputs(COLOR_RED, stderr);
			highlight = 1;
		}

		if(*str == ' ' || *str == '[' || *str == ']' || *str == '+') {
			if(highlight) {
				fputs(COLOR_NONE, stderr);
				highlight = 0;
			}
			arg++;
		}
		fputc(*str++, stderr);

		if(highlight)
			wordLength++;

		counter++;
	}

	fputs("\n"COLOR_NONE, stderr);
	fputs("     |   ", stderr);

	for(unsigned int i = 0; i < error->offset; i++)
		fputc(' ', stderr);

	fputs(COLOR_RED "^", stderr);
	for(unsigned int i = 0; i < wordLength - 1; i++)
		fputc('~', stderr);
	fputs("\n"COLOR_NONE, stderr);
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

	unsigned int currentLine = 1;
	pc = 0;
	size_t fileOffset = 0;
	while(fileOffset < fileSize) {
		error->line = currentLine++;
		if(assembleString(file + fileOffset, &code[pc], labels, labelCount, &pc, error) != 0)
			break;
		fileOffset += strlen(file + fileOffset) + 1;
	}

	if(input->needHeader)
		writeHeader(input->out);

	if(!error->message)
		fwrite(code, sizeof(struct ProcessorInstruction), codeCount, input->out);
	else
		printError(file + fileOffset, error);

	free(file);
	free(code);
	free(labels);

	return error->message ? -1 : 0;
}

