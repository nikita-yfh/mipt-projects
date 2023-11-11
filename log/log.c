#include "log.h"

#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

#include "colors.h"

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
	{'W', "\e[1;33m", 0xEEAA00},
	{'E', "\e[1;31m", 0xFF0000},
	{'F', "\e[1;31m", 0xFF0000}
};

static void printLineBegin(FILE *file, enum LogLevel level, const char *function, int line) {
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
	else {
		static int isFileBegin = 1;
		if(isFileBegin) {
			fputs("<pre>\n", file);
			isFileBegin = 0;
		}
		snprintf(colorFormat, 4096, "<font color='#%06X'>", logLevels[level].colorCode);
	}

	fprintf(file, "[%04lld.%03lld] %20s:%-3d %s[%c] ", seconds, milliseconds,
		function, line, colorFormat, logLevels[level].letter);
}

static void printLineEnd(FILE *file) {
	if(isatty(fileno(file)))
		fprintf(file, "\e[0m\n");
	else
		fprintf(file, "</font>\n");
}

int _vprintLog(enum LogLevel level, const char *function, int line, const char* fmt, va_list args) {
	printLineBegin(stderr, level, function, line);
	int ret = vfprintf(stderr, fmt, args);
	printLineEnd(stderr);
	return ret;
}

int _printLog(enum LogLevel level, const char *function, int line, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ret = _vprintLog(level, function, line, fmt, args);
	va_end(args);
	return ret;
}

static int printGraphSvg(FILE *file, const char *commandLine) {

	FILE *dot = popen(commandLine, "r");

	bool needWrite = false;

	char line[1024];
	while(fgets(line, sizeof(line), dot)) {
		if(strncmp(line, "<svg", 4) == 0)
			needWrite = true;
		if(needWrite)
			fputs(line, file);
	}

	pclose(dot);

	return 0;
}

static int printGraph(FILE *file, const char *tmpFile) {
	if(isatty(fileno(file))) {
		fprintf(file, COLOR_MAGENTA"<image is missing, see HTML log>\n"COLOR_NONE);
		return 0;
	}

	char commandLine[PATH_MAX] = "";
	snprintf(commandLine, sizeof(commandLine), "dot -Tsvg '%s'", tmpFile);

	return printGraphSvg(file, commandLine);
}

int _vinsertGraphLog(enum LogLevel level, const char *tmpFile,
		const char *function, int lineNumber, const char *titleFmt, va_list args) {
	_vprintLog(level, function, lineNumber, titleFmt, args);
	return printGraph(stderr, tmpFile);
}

int _insertGraphLog(enum LogLevel level, const char *file,
		const char *function, int lineNumber, const char *titleFmt, ...) {
	va_list args;
	va_start(args, titleFmt);
	int ret = _vinsertGraphLog(level, file, function, lineNumber, titleFmt, args);
	va_end(args);
	return ret;
}

