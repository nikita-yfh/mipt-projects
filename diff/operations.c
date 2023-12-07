#include "operations.h"

#include <math.h>
#include <stddef.h>

static double funcAdd   (double a, double b) { return a + b; }
static double funcSub   (double a, double b) { return a - b; }
static double funcMul   (double a, double b) { return a * b; }
static double funcDiv   (double a, double b) { return a / b; }
static double funcLog   (double a, double b) { return log(b) / log(a); }
static double funcCtg   (double a, double  ) { return tan(M_PI / 2.0 - a); }
static double funcArcctg(double a, double  ) { return M_PI / 2.0 - atan(a); }

static struct Operation operations[] = {
	{"+",            OPERATION_BINARY, funcAdd},
	{"-",            OPERATION_BINARY, funcSub},
	{"*",            OPERATION_BINARY, funcMul},
	{"/",            OPERATION_BINARY, funcDiv},
	{"mod",          OPERATION_BINARY, fmod},
	{"log",          OPERATION_BINARY, funcLog},
	{"ln",           OPERATION_BINARY, log},
	{"lg",           OPERATION_BINARY, log10},
	{"sin",          OPERATION_UNARY,  sin},
	{"cos",          OPERATION_UNARY,  cos},
	{"tg",           OPERATION_UNARY,  tan},
	{"ctg",          OPERATION_UNARY,  funcCtg},
	{"arcsin",       OPERATION_UNARY,  asin},
	{"arccos",       OPERATION_UNARY,  acos},
	{"arctg",        OPERATION_UNARY,  atan},
	{"arcctg",       OPERATION_UNARY,  funcArcctg},
};

static const int OPERATION_COUNT = sizeof(operations) / sizeof(*operations);

const char *getOperationName(operation_t operation) {
	if(operation < 0 || operation >= OPERATION_COUNT)
		return NULL;
	return operations[operation].name;
}
