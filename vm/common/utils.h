#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

void cutLines(char *data);
unsigned int countLines(const char *data);
size_t getFileSize(FILE *file);
int stricmp(const char *a, const char *b);

#endif
