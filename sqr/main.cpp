#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <assert.h>

#include "solver.h"
#include "io.h"
#include "utils.h"
#include "test.h"

static struct option longOpt[] = {
	{"version",			no_argument,		NULL,	'v'},
	{"help",			no_argument,		NULL,	'h'},
	{"coefs",			required_argument,	NULL,	'c'},
	{"tests",			required_argument,	NULL,	't'},
	{0}
};

static const char *shortOpt = "vhc:t:";

int main(int argc, char **argv) {
	unsigned int factor = 0;
	double *coefs = NULL;

	int option = 0;
	int optionIndex = 0; // TODO: a separate function?
	while((option = getopt_long(argc, argv, shortOpt, longOpt, &optionIndex)) != -1) {
		switch(option) {
		case 'h':
			printHelp(argv[0]);
			return 0;
		case 'v':
			printVersion();
			return 0;
		case 'c':
			if(parseSeparatedStringDouble(optarg, &coefs, &factor)) {
				fprintf(stderr, "Строка коэффициентов кривая");
				return -1;
			}

			assert(factor != 0);
			factor--;

			if(factor > MAX_FACTOR) {
				fprintf(stderr, "Уравнения степени %u не поддерживаются", factor);
				free(coefs);
				return -1;
			}

			break;
		case 't':
			return runTests(optarg);
		default:
			printHelp(argv[0]);
			return -1;
		}
	}

	if(!coefs) {
		printVersion();

		if(inputFactor(&factor) == EOF)
			return 0;

		unsigned int coefCount = factor + 1;
		coefs = (double*) calloc(coefCount, sizeof(double));

		printFormat(factor);
		printLetters(factor);

		if(inputArray(coefs, coefCount) == EOF) {
			free(coefs);
			return 0;
		}
	}

	double *roots = (double*) calloc(factor, sizeof(double)); // TODO: dynamic memory allocation?
	int rootCount = solveEquation(factor, coefs, roots);
	printRoots(roots, rootCount);
	free(roots);

	free(coefs);

	return 0;
}

