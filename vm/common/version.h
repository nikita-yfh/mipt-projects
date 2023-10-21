#ifndef VERSION_H
#define VERSION_H

#include <stdio.h>

#define VERSION "1.0.0"

#define VERSION_CODE 1
#define MIN_VERSION_CODE 1

#define SIGNATURE 0x6866796E

void printVersion(const char *programName);

void writeHeader(FILE *file);
int checkHeader(FILE *file);

#endif

