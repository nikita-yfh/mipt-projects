#include "log.h"

#include <sys/time.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

static long long currentTimestamp() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000; // calculate milliseconds
    return milliseconds;
}

struct LogLevelS {
	char letter;
	const char *color;
};

static struct LogLevelS logLevels[] = {
	{'V', "\e[1;37m"},
	{'D', "\e[1;35m"},
	{'I', "\e[1;32m"},
	{'W', "\e[1;33m"},
	{'E', "\e[0;31m"},
	{'F', "\e[1;31m"}
};

static void printLineBegin(enum LogLevel level) {
	assert(level >= LOG_VERBOSE && level <= LOG_FATAL);

	static long long timeBegin = 0;
	if(timeBegin == 0)
		timeBegin = currentTimestamp();

	long long currentTime = currentTimestamp() - timeBegin;

	long long milliseconds = currentTime % 1000;
	long long seconds      = currentTime / 1000;

	printf("[%04lld.%03lld] %s[%c] ", seconds, milliseconds,
		logLevels[level].color, logLevels[level].letter);
}

static void printLineEnd() {
	puts("\e[0m");
}

int vprintLog(enum LogLevel level, const char* fmt, va_list args) {
	printLineBegin(level);
	int ret = vprintf(fmt, args);
	printLineEnd();
	return ret;
}

int printLog(enum LogLevel level, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ret = vprintLog(level, fmt, args);
	va_end(args);
	return ret;
}
