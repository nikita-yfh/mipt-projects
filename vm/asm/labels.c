#include "labels.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "utils.h"

static int checkLabel(const char *buffer, unsigned int *pc, struct Label *label) {
	assert(buffer);
	assert(label);
	assert(pc);

	if(*buffer == '\0')
		return 0;

	if(*buffer != ':') {

		while(*buffer == ' ')
			buffer++;

		if(*buffer != ';')
			(*pc)++;

		return 0;
	}
	buffer++;

	buffer = skipBlanks(buffer);

	label->name = buffer;
	label->length = (unsigned int) strlenToChar(buffer, ' ');
	label->offset = *pc;

	return 1;
}

int labelsGetOffset(const struct Labels *labels, const char *str, unsigned int *offset) {
	for(unsigned int labelIndex = 0; labelIndex < labels->count; labelIndex++) {
		struct Label *label = &labels->labels[labelIndex];
		if(strncmp(str, label->name, label->length + 1) == 0) {
			*offset = label->offset;
			return 0;
		}
	}
	return -1;
}

unsigned int labelsCheckFileAndCountCode(struct Labels *labels, const char *lines, unsigned int lineCount) {
	unsigned int pc = 0;

	for(unsigned int lineNumber = 0; lineNumber < lineCount; lineNumber++) {
		if(checkLabel(lines, &pc, &labels->labels[labels->count]))
			labels->count++;

		lines = skipNull(lines);
	}

	return pc;
}

void labelsCreate(struct Labels *labels, unsigned int maxCount) {
	assert(labels);

	labels->labels = (struct Label*) calloc(maxCount, sizeof(struct Label));
	labels->count = 0;

	assert(labels->labels);
}

void labelsDelete(struct Labels *labels) {
	free(labels->labels);
}

