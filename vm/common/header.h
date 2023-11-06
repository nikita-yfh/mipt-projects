#ifndef VERSION_H
#define VERSION_H

#include <stdio.h>
#include <stdint.h>

#define VERSION "1.3.0"

#define VERSION_CODE 6
#define MIN_VERSION_CODE 5

#define SIGNATURE 0x6866796E

#define NIKITA "nikita-yfh"
#define PROGRAM "ASM-yfh"

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

