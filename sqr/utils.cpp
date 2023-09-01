#include <stdio.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "utils.h"

int isZero(double value) {
	return fabs(value) < DBL_EPSILON;
}

int invalidInput() {
	int c = '\n';
	while((c = getchar()) != '\n') {
		if(c == EOF)
			return EOF;
	}

	printf("Неверный ввод. Повторите попытку: ");
	return 0;
}

// TODO: allow user to choose if he wants dynamic memory allocation or not :)
int parseSeparatedStringDouble(const char *origStr, double **coefs, unsigned int *size) {
	assert(origStr);
	assert(coefs);
	assert(size);

	if(!*origStr)
		return -1;

	char *str = strdup(origStr);

	char *c = str;

	*size = 1;
	while(*c) { // Заменяем все запятые на 0-терминатор
		if(*c == ',') {
			*c = '\0';
			(*size)++;
		}
		c++;
	}

	*coefs = (double*) calloc(*size, sizeof(double));

	c = str;
	for(unsigned int coef = 0; coef < *size; coef++) {
		if(sscanf(c, "%lg", &(*coefs)[coef]) != 1) {
			free(*coefs);
			free(str);
			return -1;
		}
		while(*c) c++;
		c++;
	}

	free(str);

	return 0;
}
