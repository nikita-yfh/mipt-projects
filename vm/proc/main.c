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
		"    -m, --memory=<value> Memory size\n"
		"    -o, --offset=<value> Program executing offset\n"
		"    -V, --video=<width>,<height>,<offset>\n"
		"                         Set video mode\n"
		"    -H, --no-header      Skip header reading\n"
		"    -I, --ignore-header  Ignore file header\n\n", programName);

}

static int parseNumber(const char *str, arg_t *arg) {
	if(str[0] == '0' && str[1] == 'x')
		return sscanf(str + 2, "%x", arg) != 1;
	return sscanf(str, "%u", arg) != 1;
}

static int parseVideoMode(const char *_str, struct ProcessorInput *input) {
	char str[1024];
	strncpy(str, _str, sizeof(str));

	const char *widthStr  = strtok(str,  ",");
	const char *heightStr = strtok(NULL, ",");
	const char *offsetStr = strtok(NULL, ",");

	if(offsetStr == NULL || strtok(NULL, ","))
		return -1; // invalid arg count
	
	if(parseNumber(widthStr,  &input->videoWidth)  ||
	   parseNumber(heightStr, &input->videoHeight) ||
	   parseNumber(offsetStr, &input->videoOffset))
		return -1;

	return 0;
}

static int parseArgs(int argc, char *argv[], struct ProcessorInput *input) {
	assert(input);

	const char *shortOptions = "hvHIlm:o:V:";
	struct option longOptions[] = {
		{"help",           no_argument,       NULL, 'h'},
		{"version",        no_argument,       NULL, 'v'},
		{"no-header",      no_argument,       NULL, 'H'},
		{"ignore-header",  no_argument,       NULL, 'I'},
		{"log",            no_argument,       NULL, 'l'},
		{"memory",         required_argument, NULL, 'm'},
		{"offset",         required_argument, NULL, 'o'},
		{"video",          required_argument, NULL, 'V'},
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
		case 'o':
			if(parseNumber(optarg, &input->initCodeOffset)) {
				fprintf(stderr, "Invalid program executing offset\n");
				return -1;
			}
			break;
		case 'm':
			if(parseNumber(optarg, &input->memorySize)) {
				fprintf(stderr, "Invalid memory value\n");
				return -1;
			}
			break;
		case 'V':
			if(parseVideoMode(optarg, input)) {
				fprintf(stderr, "Invalid video mode\n");
				return -1;
			}
			input->videoEnable = 1;
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
