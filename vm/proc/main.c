#include "instruction.h"

#include <getopt.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "header.h"
#include "processor.h"
#include "log.h"

static void printHelp(const char *programName) {
	printf(
		"Usage: %s [-vhH] <file.yb>\n"
		"    -v, --version        Print author and version\n"
		"    -h, --help           Print usage\n"
		"    -l, --log            Print verbose log\n"
		"    -H, --no-header      Skip header reading"
		"    -I, --ignore-header  Ignore file header\n\n", programName);

}

static int parseArgs(int argc, char *argv[], struct ProcessorInput *input) {
	assert(input);

	const char *shortOptions = "hvHIl";
	struct option longOptions[] = {
		{"help",           no_argument,       NULL, 'h'},
		{"version",        no_argument,       NULL, 'v'},
		{"no-header",      no_argument,       NULL, 'H'},
		{"ignore-header",  no_argument,       NULL, 'I'},
		{"log",            no_argument,       NULL, 'l'},
		{NULL,             0,                 NULL,  0 }
	};

	int optionIndex = 0, option = 0;
	while((option = getopt_long(argc, argv, shortOptions, longOptions, &optionIndex)) != -1) {
		switch(option) {
		case 'h':
			printHelp(argv[0]);
			return 0;
		case 'H':
			input->skipHeader = 0;
			break;
		case 'I':
			input->ignoreHeader = 1;
			break;
		case 'v':
			printVersion(argv[0]);
			return 0;
		case 'l':
			input->verboseLog = 1;
			break;
		default:
			fprintf(stderr,
                "Unrecognized option: '-%c'\n", optopt);
			return -1;
		}
	}

	if(optind != argc - 1) {
		printHelp(argv[0]);
		return -1;
	}

	input->fileName = argv[optind];

	return 1;
}

int main(int argc, char *argv[]) {
	struct ProcessorInput input = {};

	int ret = parseArgs(argc, argv, &input);
	if(ret != 1)
		return ret;

	input.file = fopen(input.fileName, "rb");

	if(!input.file)
		printLog(LOG_ERROR, "Failed to open file '%s'", input.fileName);
	else {
		struct Processor processor = {};
		if(!processorCreate(&processor, &input)) {
			processorRun(&processor);
		}
		processorDelete(&processor);
	}

	if(input.file)
		fclose(input.file);

	return input.file == 0;
}
