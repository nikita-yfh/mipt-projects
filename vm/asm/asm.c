#include "instruction.h"

#include <getopt.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "text.h"

struct Input {
	const char *inputFile;
	const char *outputFile;
};

static void printHelp(const char *programName) {
	printf(
		"Usage: %s -o <input.ys> <output.yb> [-v] [-h]\n"
		"    -o, --output <file>  Set file to process\n"
		"    -v                   Print author and version\n"
		"    -h                   Print usage\n\n", programName);

}

static int parseArgs(int argc, char *argv[], struct Input *input) {
	assert(input);

	const char *shortOptions = "o:hv";
	struct option longOptions[] = {
		{"output",   required_argument, NULL, 'o'},
		{"help",     no_argument,       NULL, 'h'},
		{"version",  no_argument,       NULL, 'v'},
		{NULL,       0,                 NULL, 0}
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
		case 'v':
			printVersion(argv[0]);
			return 0;
		default:
			fprintf(stderr,
                "Unrecognized option: '-%c'\n", optopt);
			return -1;
		}
	}

	if(!input->outputFile)
		return -1;

	if(optind != argc - 1)
		return -1;

	input->inputFile = argv[optind];

	return 0;
}

int main(int argc, char *argv[]) {
	struct Input input = {};

	if(parseArgs(argc, argv, &input)) {
		printHelp(argv[0]);
		return -1;
	}

	return 0;
}
