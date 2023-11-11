#ifndef LOG_H
#define LOG_H

#include <stdarg.h>

enum LogLevel {
	LOG_VERBOSE,
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
	LOG_FATAL
};

#define printLog(level, ...) \
	_printLog(level, __func__, __LINE__, __VA_ARGS__)

#define insertGraphLog(level, tmpfile, ...) \
	_insertGraphLog(level, tmpfile, __func__, __LINE__, __VA_ARGS__)

int _insertGraphLog(enum LogLevel level, const char *tmpFile,
	const char *function, int line, const char *titleFmt, ...);
int _vinsertGraphLog(enum LogLevel level, const char *tmpFile,
	const char *function, int line, const char *titleFmt, va_list args);

int _printLog(enum LogLevel level, const char *function, int line, const char* fmt, ...);
int _vprintLog(enum LogLevel level, const char *function, int line, const char* fmt, va_list args);

#endif
