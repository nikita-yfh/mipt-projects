#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "solver.h"

#include "utils.h"

static int solveBase(const double *coefs, double * roots) {

	assert(coefs);
	assert(roots);

	return isZero(coefs[0]) ? ROOTS_ANY : ROOTS_NO;
}

static int solveLin(const double *coefs, double *roots) {

	assert(coefs);
	assert(roots);

	roots[0] = -coefs[1] / coefs[0];
	return 1;
}

static int solveSq(const double *coefs, double *roots) {

	assert(coefs);
	assert(roots);

	double a = coefs[0], b = coefs[1], c = coefs[2];
	double d = b*b - 4.0*a*c;
	if(isZero(d)) {
		roots[0] = -b / (2.0*a);
		return 1;
	} else if(d > 0.0) {
		double sqd = sqrt(d);
		roots[0] = (-b + sqd) / (2.0*a);
		roots[1] = (-b - sqd) / (2.0*a);
		return 2;
	} else 
		return ROOTS_NO;
}

int solveEquation(unsigned int factor, const double *coefs, double *roots) {

	assert(0 < factor && factor <= MAX_FACTOR);
	assert(coefs);
	assert(roots);

	typedef int (*solveFunction_t)(const double*, double*);

	static solveFunction_t solveFunctions[MAX_FACTOR + 1] = {
		&solveBase,
		&solveLin,
		&solveSq
	};

	while(isZero(coefs[0]) && factor > 0) {
		coefs++;
		factor--;
	}
	return solveFunctions[factor](coefs, roots);
}

