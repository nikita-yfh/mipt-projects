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
#include "labels.h"

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

	if(str[0] == '0' && str[1] == 'x') {
		if(sscanf(str + 2, "%x", value) != 1) {
			error->message = "invalid number value";
			return -1;
		}
		return 0;
	} else if(strchr(str, '.') != NULL) {
		if(sscanf(str, "%f", (float*) value) != 1) {
			error->message = "invalid float value";
			return -1;
		}
		return 0;
	}

	if(sscanf(str, "%d", (int*) value) != 1) {
		error->message = "invalid number value";
		return -1;
	}
	return 0;
}

static int readLabel(const char *str, const struct Labels *labels, arg_t *value, struct AsmError *error) {

	assert(labels);
	assert(error);
	assert(value);
	assert(str);

	str = skipChar(str, ':');

	if(labelsGetOffset(labels, str, value)) {
		error->message = "invalid label or offset";
		return -1;
	}

	return 0;
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
						const struct Labels *labels, struct AsmError *error) {
	if(!argument)
		return 0;

	assert(instruction);
	assert(labels);
	assert(error);

	if(*argument == ':') { // label

		if(instruction->flags & FLAG_IMM) {
			error->message = "instruction already has immutable const";
			return -1;
		}

		instruction->flags |= FLAG_IMM;
		return readLabel(argument, labels, &instruction->immutable, error);

	} else if(isdigit(*argument)) { // immutable const
									//
		if(instruction->flags & FLAG_IMM) {
			error->message = "instruction already has immutable const";
			return -1;
		}

		instruction->flags |= FLAG_IMM;
		return readNumber(argument, &instruction->immutable, error);

	} else if(isalpha(*argument)) { // register

		if(instruction->flags & FLAG_REG) {
			error->message = "instruction already has register";
			return -1;
		}

		instruction->flags |= FLAG_REG;
		return readRegister(argument, &instruction->reg, error);

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
			error->column = (unsigned int) (beginBracket - line);
		else
			error->column = (unsigned int) (endBracket   - line);
		return -1;
	}

	const char *nextBeginBracket = strchr(beginBracket + 1, '[');
	const char *nextEndBracket   = strchr(endBracket   + 1, ']');

	if(!nextBeginBracket && !nextEndBracket) {
		instruction->flags |= FLAG_MEM;
		return 0;
	}

	error->message = "mismatched bracket";

	if(nextBeginBracket)
		error->column = (unsigned int) (nextBeginBracket - line);
	if(nextEndBracket)
		error->column = (unsigned int) (nextEndBracket   - line);

	return -1;
}

static int assembleString(const char *buffer, struct ProcessorInstruction *instruction,
					const struct Labels *labels, unsigned int *pc, struct AsmError *error) {
	assert(buffer);
	assert(error);
	assert(instruction);
	assert(labels);

	if(!*buffer)
		return 0;

	char line[1024]; // TODO: extract to const (also, why even copy it?)
	strncpy(line, buffer, sizeof(line));
	stripCommand(line);

	if(!*line)
		return 0;

	if(*line == ':') {
		const char *label = strtok(line + 1, " ");
		while(*label) {
			if(!isalpha(*label) && !isdigit(*label) && *label != '_') {
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
			error->column = (unsigned int)(commandString - line);
			error->message = "invalid command";
		}

		for(int arg = 0; arg < 2; arg++) {
			const char *argString = strtok(NULL, div);
			if(readArgument(argString, instruction, labels, error)) {
				error->column = (unsigned int)(argString - line);
				return -1;
			}
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
		if(highlight == 0 && counter == error->column) {
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

	for(unsigned int i = 0; i < error->column; i++)
		fputc(' ', stderr);

	fputs(COLOR_RED "^", stderr);
	if(wordLength)
		for(unsigned int i = 0; i <= wordLength; i++)
			fputc('~', stderr);
	fputs("\n"COLOR_NONE, stderr);
}

int assembleFile(struct AsmInput *input, struct AsmError *error) {
	size_t fileSize = getFileSize(input->in);

        // TODO: such low-lever operations like calloc have no place
        //       in such high-level function like assembleFile.
	char *file = (char*) calloc(fileSize, sizeof(char*));
	fread(file, fileSize, 1, input->in);

        // TODO: same thing here:
	struct Labels labels = {};
	labelsCreate(&labels, countLines(file));

	cutLines(file);
	unsigned int codeCount = labelsCheckFileAndCountCode(&labels, file, fileSize);

        // TODO: Why not separate assembling in buffer from assembling to file?
	struct ProcessorInstruction *code = (struct ProcessorInstruction*)
				calloc(codeCount, sizeof(struct ProcessorInstruction));

	unsigned int pc = 0;
	unsigned int lineNumber = 1;
	const char *fileEnd = file + fileSize;
	const char *line = NULL;

	for(line = file; line < fileEnd; line = skipNull(line)) {
		error->line = lineNumber++;
		if(assembleString(line, &code[pc], &labels, &pc, error) != 0)
			break;
	}

	if(!input->skipHeader)
		writeHeader(input->out);

	if(!error->message)
		fwrite(code, sizeof(struct ProcessorInstruction), codeCount, input->out);
	else
		printError(line, error);

	free(file);
	free(code);
	labelsDelete(&labels);

	return error->message ? -1 : 0;
}

