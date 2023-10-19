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

static uint32_t readNumber(const char *str, struct AsmError *error) {
	assert(error);
	assert(str);

	uint32_t value = 0;

	if(sscanf(str, "%d", (int*)   &value) != 1 &&
	   sscanf(str, "%f", (float*) &value) != 1) {
		error->message = "invalid number value";
		return 0;
	}

	return value;
}

static reg_t readRegister(const char *str, struct AsmError *error) {
	assert(error);
	assert(str);

	reg_t reg = stringToRegister(str);

	if(reg == REG_INVALID)
		error->message = "invalid register name";

	return reg;
}

static uint32_t readNextLabel(const struct Label *labels, unsigned int labelCount,
									struct AsmError *error) {
	assert(labels);
	assert(error);

	unsigned int offset = UINT_MAX;

	const char *jumpStr = strtok(NULL, " ");
	sscanf(jumpStr, "%u", &offset);

	for(unsigned int label = 0; label < labelCount; label++)
		if(strcmp(labels[label].name, jumpStr) == 0)
			offset = labels[label].offset;

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
		case C_POP_REG: {
			const char *str = strtok(NULL, " ");

			if(!str) {
				error->message = "to few arguments";
				return -1;
			}

			instruction->arg = readRegister(str, error);
			}
			break;
		case C_PUSH: {
			const char *str = strtok(NULL, " ");

			if(!str) {
				error->message = "to few arguments";
				return -1;
			}

			if(isalpha(*str)) {
				instruction->command = C_PUSH_REG;
				instruction->arg = readRegister(str, error);
			} else
				instruction->arg = readNumber(str, error);
			}
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
	fprintf(stderr, "%s:%u: " COLOR_RED "Error:" COLOR_NONE " %s\n",
			error->file, error->line, error->message);

	unsigned int arg = 0;
	int highlight = 0;

	fprintf(stderr, "%4u |   ", error->line);

	unsigned int redBegin = 0;
	unsigned int redCount = 0;
	while(*str) {
		if(highlight == 0 && arg == error->arg) {
			fputs(COLOR_RED, stderr);
			highlight = 1;
		}

		if(*str == ' ') {
			if(highlight) {
				fputs(COLOR_NONE, stderr);
				highlight = 0;
			}
			arg++;
		}
		fputc(*str++, stderr);

		if(highlight)
			redCount++;
		else if(!redCount)
			redBegin++;
	}

	fputs("\n"COLOR_NONE, stderr);
	fputs("     |   ", stderr);

	for(unsigned int i = 0; i < redBegin; i++)
		fputc(' ', stderr);

	fputs(COLOR_RED "^", stderr);
	for(unsigned int i = 0; i < redCount - 1; i++)
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

	if(!error->message)
		fwrite(code, sizeof(struct ProcessorInstruction), codeCount, input->out);
	else
		printError(file + fileOffset, error);

	free(file);
	free(code);
	free(labels);

	return error->message ? -1 : 0;
}

