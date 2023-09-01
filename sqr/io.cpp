#include <stdlib.h>
#include <assert.h>

#include "io.h"

#include "solver.h"
#include "utils.h"
#include "version.h"

#include <stdio.h>

int inputFactor(unsigned int *factor) {
	assert(factor);

	printf("Введите степень уравнения от 1 до %u: ", MAX_FACTOR);

	while(scanf("%u", factor) != 1 || 1 > *factor || *factor > MAX_FACTOR)
		if(invalidInput() == EOF)
			return EOF;

	return 0;
}

void printFormat(unsigned int factor) {
	assert(factor != 0);

	printf("Уравнение вида ");
	char letter = 'a';
	for(unsigned int f = factor; f >= 2; f--)
		printf("%cx^%u + ", letter++, f);

	printf("%cx + %c = 0\n", letter, letter + 1);
}

void printLetters(unsigned int factor) {
	printf("\nВведите ");
	char letter = 'a';
	for(unsigned int index = 0; index < factor; index++)
		printf("%c, ", letter++);
	printf("%c: ", letter);
}




int inputArray(double *array, unsigned int size) {
	assert(array);

	for(unsigned int index = 0; index < size; index++)
		while(scanf("%lg", &array[index]) != 1)
			if(invalidInput() == EOF)
				return EOF;

	return 0;
}

void printArray(const double *array, unsigned int size) {
	assert(array);

	printf("{ ");
	for(unsigned int index = 0; index < size - 1; index++)
		printf("%lg; ", array[index]);
	printf("%lg }\n", array[size - 1]);
}

void printRoots(const double *roots, int rootCount) {
	assert(roots);
	assert(rootCount >= -1);

	switch(rootCount) {
	case ROOTS_ANY:
		printf("Бесконечное множество решений\n");
		break;
	case ROOTS_NO:
		printf("Нет решений\n");
		break;
	default:
		printArray(roots, (unsigned int) rootCount);
		break;
	}
}

void printVersion() {
	printf("Equation solver v%s\n"
	       "Copyright (c) 2023 nikita-yfh\n\n", VERSION);
}

void printHelp(const char *programName) {
	printf(
		"Использование: %s [-h] [-v] [-i] [-t] <файл> [-c] <коэффициенты>\n\n"
		"Решает уравнение любой степени от 1 до %u\n"
		"При отсутствии параметра -c включается интерактивный режим ввода\n\n"
		"Параметры:\n"
		" -h, --help            показать эту справку и выйти\n"
		" -v, --version         показать информацию о версии и выйти\n"
		" -t, --tests=ФАЙЛ      тестировать программу на тестах из файла\n"
		" -c, --coefs=a,b,c...  список коэффициентов уравнения\n\n\n"
		"Формат тестов:\n"
		"FACTOR <степень уравнения> COEFS <a> <b> <c> ... ROOTC <количество корней> ROOTS <x1> <x2> ...\n\n",
			programName, MAX_FACTOR);
}
