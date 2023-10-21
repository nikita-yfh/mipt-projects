#include "version.h"

#include <stdio.h>
#include <stdint.h>

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

void printVersion(const char *programName) {
	printf(COLOR_GREEN"%s"COLOR_NONE, nameArt);
	printf("This is "COLOR_BLUE"%s"COLOR_NONE" of "COLOR_MAGENTA PROGRAM COLOR_NONE
		" v"VERSION" by "COLOR_GREEN"NIKITA"COLOR_NONE "(C)\n\n", programName);
}

struct Header {
	uint32_t signature;
	uint32_t version;
};

void writeHeader(FILE *file) {
	struct Header header = {SIGNATURE, VERSION_CODE};
	fwrite(&header, sizeof(header), 1, file);
}

int checkHeader(FILE *file) {
	struct Header header = {};
	if(!fread(&header, sizeof(header), 1, file))
		return -1;
	if(header.signature != SIGNATURE)
		return -1;
	if(header.version < MIN_VERSION_CODE || header.version > VERSION_CODE)
		return -1;
	return 0;
}
