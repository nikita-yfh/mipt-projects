#include <signal.h>
#include <assert.h>
#include <stdlib.h>

#include "signalHandler.h"

static ProcessOutput *Output;

static void sighandler(int signum) {
	Output->signum = signum;
	exit(1);
}

void bindSignals(ProcessOutput *output) {
	Output = output;
	signal(SIGINT,     sighandler); // TODO: sigaction
	signal(SIGFPE,     sighandler);
	signal(SIGILL,     sighandler);
	signal(SIGSEGV,    sighandler);
	signal(SIGBUS,     sighandler);
	signal(SIGABRT,    sighandler);
}

const char *getSignalDescription(int signum) {
	switch(signum) {
	case SIGINT:
		return "SIGINT: Сигнал прерывания (Ctrl-C) с терминала";
	case SIGFPE:
		return "SIGFPE: Ошибочная арифметическая операция";
	case SIGILL:
		return "SIGILL: Недопустимая инструкция процессора";
	case SIGSEGV:
		return "SIGSEGV: Нарушение при обращении в память";
	case SIGBUS:
		return "SIGBUS: Неправильное обращение в физическую память";
	case SIGABRT:
		return "SIGABRT: Сигнал, посылаемый функцией abort()";
	default:
		assert(0);
	}
}
