#ifndef VERSION_H
#define VERSION_H

#include <stdio.h>
#include <stdint.h>

#define VERSION "1.0.0"

#define VERSION_CODE 1
#define MIN_VERSION_CODE 1

#define SIGNATURE 0x6866796E

typedef uint32_t version_t;
typedef uint32_t signature_t;

struct Header {
	signature_t signature;
	version_t   version;
};

void printVersion(const char *programName);

void writeHeader(FILE *file);
int readHeader(FILE *file, struct Header *header);

#endif

