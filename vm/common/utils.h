#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

unsigned int cutLines(char *data);
size_t getFileSize(FILE *file);
int stricmp(const char *a, const char *b);

const char *skipChar(const char *str, char c);
const char *skipBlanks(const char *str);
unsigned long strlenToChar(const char *str, char c);

const char *skipNull(const char *str);

#endif
