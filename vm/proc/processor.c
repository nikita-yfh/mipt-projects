#include "processor.h"
#include "header.h"
#include "log.h"
#include "utils.h"

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

	processor->pc = input->startCodeOffset;

	return 0;
}

int processorDelete(struct Processor *processor) {
	printLog(LOG_INFO, "Exiting...");

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

int processorVerify(struct Processor *processor) {
	return stackVerify(&processor->mainStack) || stackVerify(&processor->callStack);
}

void processorDump(struct Processor *processor) {
}
