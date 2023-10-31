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

int _printLog(enum LogLevel level, const char *function, int line, const char* fmt, ...);
int _vprintLog(enum LogLevel level, const char *function, int line, const char* fmt, va_list args);

#endif
