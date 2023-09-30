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

int printLog(enum LogLevel level, const char* fmt, ...);
int vprintLog(enum LogLevel level, const char* fmt, va_list args);

#endif
