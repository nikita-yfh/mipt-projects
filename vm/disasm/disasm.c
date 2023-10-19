#include "disasm.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "instruction.h"
#include "version.h"
#include "utils.h"

static int disassembleInstruction(const struct ProcessorInstruction *instruction,
						char *buffer, size_t bufferSize, struct DisasmError *error) {
	strncat(buffer, commandToString(instruction->command), bufferSize);

	switch(instruction->command) {
	case C_INVALID:
		error->message = "invalid instruction";
		return -1;
	case C_PUSH:
		char numbers[50] = {};

		union Arg {
			unsigned int u;
			int d;
			float f;
		} arg;
		arg.u = instruction->arg;

		snprintf(numbers, sizeof(numbers), " %d ; 0x%08x %f",
				arg.d, arg.u, arg.f);
		strncat(buffer, numbers, bufferSize);
		break;
	}

	return 0;
}

static int checkFileSize(size_t fileSize, struct DisasmError *error) {
	if(fileSize % sizeof(struct ProcessorInstruction) != 0) {
		error->message = "invalid file size";
		error->instruction = (unsigned int)(fileSize / sizeof(struct ProcessorInstruction));
		return -1;
	}
	return 0;
}

int disassembleFile(struct DisasmInput *input, struct DisasmError *error) {
	size_t fileSize = getFileSize(input->in);
	if(checkFileSize(fileSize, error) != 0)
		return -1;

	struct ProcessorInstruction instruction = {};

	unsigned int counter = 0;

	while(fread(&instruction, sizeof(instruction), 1, input->in)) {
		char buffer[1024] = "";
		if(disassembleInstruction(&instruction, buffer, sizeof(buffer), error))
			return -1;

		fputs(buffer, input->out);
		fputc('\n', input->out);
		counter++;
	}

	return 0;
}

