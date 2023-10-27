#include "header.h"

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "colors.h"

static const char *nameArt = 
	"    ___   _____ __  ___                ______\n"
	"   /   | / ___//  |/  /         __  __/ __/ /_\n"
	"  / /| | \\__ \\/ /|_/ / ______  / / / / /_/ __ \\\n"
	" / ___ |___/ / /  / / /_____/ / /_/ / __/ / / /\n"
	"/_/  |_/____/_/  /_/          \\__, /_/ /_/ /_/\n"
	"                             /____/\n";

#define NIKITA "nikita-yfh"
#define PROGRAM "ASM-yfh"

static_assert(MIN_VERSION_CODE <= VERSION_CODE);
static_assert(MIN_VERSION_CODE > 0);

void printVersion(const char *programName) {
	assert(programName);

	printf(COLOR_GREEN"%s"COLOR_NONE, nameArt);
	printf("This is "COLOR_BLUE"%s"COLOR_NONE" of "COLOR_MAGENTA PROGRAM COLOR_NONE
		" v"VERSION" by "COLOR_GREEN"NIKITA"COLOR_NONE "(C)\n\n", programName);
}

void writeHeader(FILE *file) {
	assert(file);

	struct Header header = {SIGNATURE, VERSION_CODE};
	fwrite(&header, sizeof(header), 1, file);
}

int readHeader(FILE *file, struct Header *header) {
	assert(file);
	assert(header);

	if(fread(header, sizeof(header), 1, file) != 1)
		return -1;
	return 0;
}


