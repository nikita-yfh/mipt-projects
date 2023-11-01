#include "processor.h"
#include "header.h"
#include "log.h"
#include "utils.h"
#include "graphics.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static int checkHeader(const struct ProcessorInput *input) {
	if(!input->skipHeader) {

		struct Header header = {};
		if(readHeader(input->file, &header)) {
			printLog(LOG_ERROR, "Failed to read file header");
			return -1;
		}

		int logLevel = input->ignoreHeader ? LOG_WARNING : LOG_ERROR;
		
		if(header.signature != SIGNATURE) {
			printLog(logLevel, "File header doesn't match (need 0x%08X, got 0x%08X)",
					SIGNATURE, header.signature);
			if(!input->ignoreHeader)
				return -1;
		}

		if(header.version < MIN_VERSION_CODE || header.version > VERSION_CODE) {
			printLog(logLevel, "File version doesn't match (need %d..%d, got %d)",
				   MIN_VERSION_CODE, VERSION_CODE, header.version);
			if(!input->ignoreHeader)
				return -1;
		}
		return sizeof(header);
	}
	return 0;
}

static int loadCode(struct Processor *processor, const struct ProcessorInput *input) {
	int headerSize = checkHeader(input);

	if(headerSize < 0)
		return -1;

	size_t fileSize = getFileSize(input->file);
	size_t codeBytes = fileSize - (unsigned) headerSize;

	if(codeBytes > MAX_CODE_SIZE) {
		printLog(LOG_ERROR, "Code size bigger then 0x%llX bytes is not supported", MAX_CODE_SIZE);
		return -1;
	}

	if(codeBytes % sizeof(struct ProcessorInstruction) != 0) {
		printLog(LOG_ERROR, "The code size is not a multiple of instruction size");
		return -1;
	}

	processor->codeSize = (arg_t) (codeBytes / sizeof(struct ProcessorInstruction));
	processor->code = calloc(processor->codeSize, sizeof(struct ProcessorInstruction));

	if(!processor->code) {
		printLog(LOG_FATAL, "Failed to allocate buffer of size 0x%llx", codeBytes);
		return -1;
	}

	if(!fread(processor->code, sizeof(struct ProcessorInstruction), processor->codeSize, input->file)) {
		printLog(LOG_FATAL, "Failed to read code from file");
		return -1;
	}

	printLog(LOG_INFO, "Loaded code of size 0x%08X", processor->codeSize);

	return 0;
}

int processorCreate(struct Processor *processor, const struct ProcessorInput *input) {
	assert(processor);
	assert(input);

	if(loadCode(processor, input))
		return -1;

	if(stackCreate(&processor->mainStack)) {
		printLog(LOG_FATAL, "Failed to create main stack");
		return -1;
	} else
		printLog(LOG_VERBOSE, "Created main stack");

	if(stackCreate(&processor->callStack)) {
		printLog(LOG_FATAL, "Failed to create call stack");
		return -1;
	} else 
		printLog(LOG_VERBOSE, "Created call stack");

	processor->memorySize = input->memorySize;
	processor->memory = calloc(processor->memorySize, sizeof(arg_t));

	if(!processor->memory) {
		printLog(LOG_FATAL, "Failed to allocate memory of size 0x%08x", processor->memorySize);
		return -1;
	}

	if(input->defaultMemValue != 0)
		memset(processor->memory, input->defaultMemValue, processor->memorySize * sizeof(arg_t));

	printLog(LOG_INFO, "Allocated memory of size 0x%08x, value 0x%02x",
			processor->memorySize, input->defaultMemValue);

	memset(processor->registers, input->defaultMemValue, sizeof(processor->registers));

	printLog(LOG_INFO, "Created %u registers, value 0x%02x",
			REG_COUNT, input->defaultMemValue);

	processor->pc = input->initCodeOffset;

	if(processor->pc >= processor->codeSize)
		printLog(LOG_WARNING, "Init code offset is bigger then last instruction index");

	if(input->videoEnable) {
		if(graphicsInit(processor->memory + input->videoOffset,
				input->videoWidth, input->videoHeight)) {
			printLog(LOG_ERROR, "Graphics init failed: %s", graphicsGetError());
			return -1;
		}
	}

	processor->videoEnable = input->videoEnable;
	processor->verboseLog = input->verboseLog;

	return 0;
}

int processorDelete(struct Processor *processor) {
	printLog(LOG_INFO, "Exiting...");

	graphicsQuit();
	printLog(LOG_VERBOSE, "Graphics quited");

	stackDelete(&processor->mainStack);
	printLog(LOG_VERBOSE, "Main stack was deleted");

	stackDelete(&processor->callStack);
	printLog(LOG_VERBOSE, "Call stack was deleted");

	free(processor->code);
	printLog(LOG_VERBOSE, "Code was deleted");

	free(processor->memory);
	printLog(LOG_VERBOSE, "Memory was deleted");

	printLog(LOG_INFO, "Goodbye!");

	return 0;
}

static const char *processorReturnCodeToString(int returnCode) {
	switch(returnCode) {
		case EXEC_INVALID_FORMAT:
			return "Invalid instruction format";
		case EXEC_INVALID_INSTRUCTION:
			return "Invalid instruction";
		case EXEC_INVALID_REGISTER:
			return "Invalid register";
		case EXEC_MATH_ERROR:
			return "Math error";
		case EXEC_STACK_OVERFLOW:
			return "Stack overflow";
		case EXEC_STACK_EMPTY:
			return "Stack is empty";
		case EXEC_OUT_OF_BOUNDS:
			return "Memory out of bounds";
		case EXEC_OK:
			return "All OK";
		case EXEC_END_PROGRAM:
			return "Program ending";
		default:
			return "Unknown error";
	}
}

int processorVerify(struct Processor *processor) {
	return stackVerify(&processor->mainStack) || stackVerify(&processor->callStack);
}

static void printInstruction(const struct ProcessorInstruction *instruction, int level) {
	char stringBuffer[128] = "";
	char hexBuffer[10];

	const unsigned char *bytes = (const unsigned char*) instruction;
	for(unsigned int i = 0; i < sizeof(struct ProcessorInstruction); i++) {
		snprintf(hexBuffer, sizeof(hexBuffer), "%02x ", *bytes);
		strcat(stringBuffer, hexBuffer);
		bytes++;
	}
	printLog(level, stringBuffer);
}

void processorDump(struct Processor *processor, int level) {
	printLog(level, "pc = %08X", processor->pc);
	printLog(level, "Current instruction: ");
	printInstruction(&processor->code[processor->pc], level);

	printLog(level, "Registers: ");
	for(reg_t reg = 0; reg < REG_COUNT; reg++)
		printLog(level, "%s = %d", registerToString(reg), processor->registers[reg]);

	printLog(level, "Main stack: ");
	stackDump(&processor->mainStack, level);

	printLog(level, "Call stack: ");
	stackDump(&processor->callStack, level);
}

int processorRun(struct Processor *processor) {
	while(processor->pc < processor->codeSize) {
		if(processor->verboseLog)
			processorDump(processor, LOG_VERBOSE);
		int ret = processorExecNextCommand(processor);

		if(processor->videoEnable) {
			if(graphicsUpdate()) {
				printLog(LOG_WARNING, "Program stopped");
				return EXEC_OK;
			}
		}

		if(ret == EXEC_END_PROGRAM)
			return EXEC_OK;
		else if(ret < 0) {
			processor->pc--;
			printLog(LOG_ERROR, "Instruction %u error: %s", processor->pc, processorReturnCodeToString(ret));
			processorDump(processor, LOG_ERROR);
			return ret;
		}
	}
	return EXEC_OK;
}

