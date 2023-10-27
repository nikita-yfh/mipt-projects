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

	input->needHeader = 1;

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
			input->needHeader = 0;
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

	input.in = fopen(input.inputFile, "r");
	input.out = fopen(input.outputFile, "wb");

	if(!input.in)
		perror("Failed to open input file");
	if(!input.out)
		perror("Failed to open output file");

	if(input.in && input.out) {
		struct AsmError error = {};
		error.file = input.inputFile;

		if(assembleFile(&input, &error) != 0)
			return -1;
	}

	if(input.in)
		fclose(input.in);
	if(input.out)
		fclose(input.out);

	return (input.in && input.out) ? 0 : -1;
}
