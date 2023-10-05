#include "text.h"

#include <stdio.h>

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
	printf("\e[1;32m%s\e[0m", nameArt);
	printf("This is \e[1;34m%s\e[0m of \e[1;35m"PROGRAM"\e[0m v"VERSION" by \e[1;32m"NIKITA"\e[0m (C)\n\n", programName);
}
