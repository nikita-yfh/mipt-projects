#include "instruction.h"

#include <getopt.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "header.h"
#include "asm.h"

static void printHelp(const char *programName) {
	printf(
		"Usage: %s -o <input.ys> <output.yb> [-vhH]\n"
		"    -o, --output <file>  Set file to process\n"
		"    -v                   Print author and version\n"
		"    -h                   Print usage\n"
		"    -H, --no-header      Don't add header to output binary\n\n", programName);

}

static int parseArgs(int argc, char *argv[], struct AsmInput *input) {
	assert(input);

	const char *shortOptions = "o:hHv";
	struct option longOptions[] = {
		{"output",       required_argument, NULL, 'o'},
		{"help",         no_argument,       NULL, 'h'},
		{"version",      no_argument,       NULL, 'v'},
		{"no-header",    no_argument,       NULL, 'H'},
		{NULL,           0,                 NULL, 0}
	};

	int optionIndex = 0, option = 0;
	while((option = getopt_long(argc, argv, shortOptions, longOptions, &optionIndex)) != -1) {
		switch(option) {
		case 'o':
			input->outputFile = optarg;
			break;
		case 'h':
			printHelp(argv[0]);
			return 0;
		case 'H':
			input->skipHeader = true;
			break;
		case 'v':
			printVersion(argv[0]);
			return 0;
		default:
			fprintf(stderr,
                "Unrecognized option: '-%c'\n", optopt);
			return -1;
		}
	}

	if(!input->outputFile) {
		printHelp(argv[0]);
		return -1;
	}

	if(optind != argc - 1) {
		printHelp(argv[0]);
		return -1;
	}

	input->inputFile = argv[optind];

	return 1;
}

int main(int argc, char *argv[]) {
	struct AsmInput input = {};

	int ret = parseArgs(argc, argv, &input);
	if(ret != 1)
		return ret;

	if(!openFiles(&input)) {
		if(assembleFile(&input) != 0)
			return -1;
	}

	return closeFiles(&input);
}
