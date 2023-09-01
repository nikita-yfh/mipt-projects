#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

#include "test.h"

#include "solver.h"
#include "utils.h"

static int runTest(FILE *file) { // TODO: maybe it's better to parse separately
	unsigned int factor = 0;
	if(fscanf(file, " FACTOR %u COEFS ", &factor) != 1 || factor > MAX_FACTOR || factor < 1)
		return RETURN_ERROR;

	unsigned int coefCount = factor + 1;
	double *coefs = (double*) calloc(coefCount, sizeof(double)); // TODO: dynamic memory fan?

	for(unsigned int index = 0; index < coefCount; index++) {
		if(fscanf(file, "%lg", &coefs[index]) != 1) {
			free(coefs);
			return RETURN_ERROR;
		}
	}

	int rootCount = 0;
	if(fscanf(file, " ROOTC %d", &rootCount) != 1 || rootCount > (int) MAX_FACTOR || rootCount < -1) {
		free(coefs);
		return RETURN_ERROR;
	}

	double *roots = NULL;

	if(rootCount > 0) {
		fscanf(file, " ROOTS ");

		roots = (double*) calloc((size_t) rootCount, sizeof(double));
		
		for(int index = 0; index < rootCount; index++) {
			if(fscanf(file, "%lg", &roots[index]) != 1) {
				free(roots);
				free(coefs);
				return RETURN_ERROR;
			}
		}
	}

	double *solveRoots = (double*) calloc(coefCount, sizeof(double));

	int solveRootCount = solveEquation(factor, coefs, solveRoots);

	int returnCode = RETURN_OK;
	if(solveRootCount != rootCount)
		returnCode = RETURN_FAIL;

	if(rootCount > 0) {
		for(int root = 0; root < rootCount; root++)
			if(!isZero(roots[root] - solveRoots[root]))
				returnCode = RETURN_FAIL;
		free(roots);
	}

	free(coefs);
	free(solveRoots);

	return returnCode;
}

int runTests(const char *fileName) {
	FILE *file = fopen(fileName, "r");
	if(!file) {
		fprintf(stderr, "Ошибка открытия файла %s\n", fileName);
		return RETURN_ERROR;
	}

	int errorCount = 0;

	printf("Запуск тестов из файла %s\n", fileName);

	for(int testNumber = 1;; testNumber++) {
		int result = runTest(file);

		switch(result) {
		case RETURN_ERROR:
			if(getc(file) != EOF) {
				fprintf(stderr, "Неверная строка %d\n", testNumber);
				return RETURN_ERROR;
			} else
				return errorCount;

		case RETURN_FAIL:
			fprintf(stderr, "Тест %d: FAIL\n", testNumber);
			errorCount++;
			break;
	
		default:
			printf("Тест %d: OK\n", testNumber);
			break;
		}
	}
}
