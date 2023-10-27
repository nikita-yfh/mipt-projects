#include "instruction.h"

#include <getopt.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "header.h"
#include "disasm.h"

struct Input {
	const char *inputFile;
	const char *outputFile;
};

static void printHelp(const char *programName) {
	printf(
		"Usage: %s -o <input.yb> <output.ys> [-v] [-h]\n"
		"    -o, --output <file>  Set file to process\n"
		"    -v                   Print author and version\n"
		"    -h                   Print usage\n\n", programName);

}

static int parseArgs(int argc, char *argv[], struct DisasmInput *input) {
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
	struct DisasmInput input = {};

	int ret = parseArgs(argc, argv, &input);
	if(ret != 1)
		return ret;

	input.in = fopen(input.inputFile, "r");
	input.out = fopen(input.outputFile, "wb");

	if(!input.in)
		perror("Failed to open input file");
	if(!input.out)
		perror("Failed to open output file");

	if(input.in && input.out) {
		struct DisasmError error = {};
		error.file = input.inputFile;

		if(disassembleFile(&input, &error) != 0)
			return -1;
	}

	if(input.in)
		fclose(input.in);
	if(input.out)
		fclose(input.out);

	return (input.in && input.out) ? 0 : -1;
}
