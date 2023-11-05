#ifndef LABELS_H
#define LABELS_H

#include <stddef.h>

struct Label {
	const char *name;
	unsigned int length;
	unsigned int offset;
};

struct Labels {
	struct Label *labels;
	unsigned int count;
};

void labelsCreate(struct Labels *labels, unsigned int maxCount);
void labelsDelete(struct Labels *labels);

int labelsGetOffset(const struct Labels *labels, const char *str, unsigned int *offset);

unsigned int labelsCheckFileAndCountCode(struct Labels *labels, const char *lines, unsigned int lineCount);

#endif

