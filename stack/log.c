#include "log.h"

#include <time.h>

long long timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000; // calculate milliseconds
    return milliseconds;
}

struct LogLevelS {
	char letter;
	const char *color;
}

static LogLevelS logLevels[] = {
	{'V', "\e[1;37m"},
	{'D', "\e[0;35m"},
	{'I', "\e[0;32m"},
	{'W', "\e[1;33m"},
	{'E', "\e[0;31m"},
	{'F', "\e[1;31m"}
}

static void printLineBegin(LogLevel level) {
	assert(level >= LOG_VERBOSE && level <= LOG_FATAL);

	static long long timeBegin = timestamp();
	int currentTime = timestamp() - timeBegin;

	int milliseconds = currentTime % 1000;
	int seconds      = currentTime / 1000;

	printf("[%04d.%03d] %s[%c] ", seconds, milliseconds,
		logLevels[level].color, logLevels[level].letter);
}

static void printLineEnd() {
	puts("\e[0m\n");
}

int vlog(LogLevel level, const char* fmt, va_list args) {
	printLineBegin(level);
	vprintf(fmt, args);
	printLineEnd();
}

int log(LogLevel level, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vlog(level, fmt, args);
	va_end(args);
}
