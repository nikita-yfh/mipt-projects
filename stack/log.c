#include "log.h"

#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

static FILE *htmlOutput = NULL;

static long long currentTimestamp() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000; // calculate milliseconds
    return milliseconds;
}

struct LogLevelS {
	char letter;
	const char *colorFormat;
	unsigned int colorCode;
};

static struct LogLevelS logLevels[] = {
	{'V', "\e[1;37m", 0x808080},
	{'D', "\e[1;35m", 0xFF00FF},
	{'I', "\e[1;32m", 0x00FF00},
	{'W', "\e[1;33m", 0xFFFF00},
	{'E', "\e[1;31m", 0xFF0000},
	{'F', "\e[1;31m", 0xFF0000}
};

static void printLineBegin(FILE *file, enum LogLevel level) {
	assert(level >= LOG_VERBOSE && level <= LOG_FATAL);

	static long long timeBegin = 0;
	if(timeBegin == 0)
		timeBegin = currentTimestamp();

	long long currentTime = currentTimestamp() - timeBegin;

	long long milliseconds = currentTime % 1000;
	long long seconds      = currentTime / 1000;

	char colorFormat[4096];
	if(isatty(fileno(file)))
		strcpy(colorFormat, logLevels[level].colorFormat);
	else
		snprintf(colorFormat, 4096, "<font color='#%06X'>", logLevels[level].colorCode);

	fprintf(file, "[%04lld.%03lld] %s[%c] ", seconds, milliseconds,
		colorFormat, logLevels[level].letter);
}

static void printLineEnd(FILE *file) {
	if(isatty(fileno(file)))
		fprintf(file, "\e[0m\n");
	else
		fprintf(file, "</font>\n");
}

static int vfprintLog(FILE *file, enum LogLevel level, const char* fmt, va_list args) {
	printLineBegin(file, level);
	int ret = vfprintf(file, fmt, args);
	printLineEnd(file);
	return ret;
}

int vprintLog(enum LogLevel level, const char* fmt, va_list args) {
	if(htmlOutput)
		vfprintLog(htmlOutput, level, fmt, args);
	return vfprintLog(stderr, level, fmt, args);
}

int printLog(enum LogLevel level, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ret = vprintLog(level, fmt, args);
	va_end(args);
	return ret;
}

int openHTMLLog(const char *file) {
	htmlOutput = fopen(file, "w");
	if(!htmlOutput)
		return -1;
	fprintf(htmlOutput, "<pre>\n");
	return 0;
}

int closeHTMLLog() {
	if(htmlOutput) {
		fprintf(htmlOutput, "</pre>\n");
		fclose(htmlOutput);
		return 0;
	}
	return -1;
}

