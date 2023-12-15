#include "operations.h"

#include <string.h>
#include <math.h>
#include <float.h>

static double toDouble(int a) {
	return (double) a;
}

static int toBool(double a) {
	return (fabs(a) < DBL_EPSILON) ? 0 : 1;
}

static double funcOr    (double a, double b) { return toDouble(toBool(a) || toBool(b)); }
static double funcAnd   (double a, double b) { return toDouble(toBool(a) || toBool(b)); }
static double funcAdd   (double a, double b) { return a + b; }
static double funcSub   (double a, double b) { return a - b; }
static double funcMul   (double a, double b) { return a * b; }
static double funcDiv   (double a, double b) { return a / b; }
static double funcLog   (double a, double b) { return log(b) / log(a); }

static double funcNot   (double a) { return toDouble(!toBool(a)); }
static double funcMinus (double a) { return -a; }
static double funcPlus  (double a) { return  a; }

const struct Operator operators[] = {
	{"|",   OPERATOR_BINARY,    1, funcOr,    NULL      },
	{"&",   OPERATOR_BINARY,    2, funcAnd,   NULL      },
	{"+",   OPERATOR_UBINARY,   3, funcAdd,   funcPlus  },
	{"-",   OPERATOR_UBINARY,   3, funcSub,   funcMinus },
	{"*",   OPERATOR_BINARY,    4, funcMul,   NULL      },
	{"/",   OPERATOR_BINARY,    4, funcDiv,   NULL      },
	{"mod", OPERATOR_BINARY,    4, fmod,      NULL      },
	{"^",   OPERATOR_BINARY,    5, pow,       NULL      },
	{"log", OPERATOR_BINARY,    5, funcLog,   NULL      },
	{"~",   OPERATOR_UNARY,    -1, NULL,      funcNot   },
	{"(",   OPERATOR_LBRACKET, -1, NULL,      NULL      },
	{")",   OPERATOR_RBRACKET, -1, NULL,      NULL      },
};

const size_t operatorCount = sizeof(operators) / sizeof(*operators);

operator_t operatorFindL(const char *text, size_t textLength) {
	for(operator_t i = 0; i < operatorCount; i++) {
		if(strlen(operators[i].name) == textLength &&
				strncmp(operators[i].name, text, textLength) == 0)
			return i;
	}
	return OPERATOR_INVALID;
}

operator_t operatorFind(const char *text) {
	for(operator_t i = 0; i < operatorCount; i++) {
		if(strcmp(operators[i].name, text) == 0)
			return i;
	}
	return OPERATOR_INVALID;
}

const struct Operator *operatorGet(operator_t number) {
	if(number >= operatorCount)
		return NULL;

	return &operators[number];
}
