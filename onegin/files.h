#ifndef FILES_H
#define FILES_H

#include <stdio.h>

char *readFile(FILE *file);
char *readTTY(FILE *file);
size_t getFileSize(FILE *file);

void writeStrings(const char **lines, size_t lineCount, FILE *output);
void writeStringsDirect(const char *data, size_t lineCount, FILE *output);

#endif

