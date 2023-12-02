#ifndef SPEAKER_H
#define SPEAKER_H

#include <stdarg.h>

int printspeakf(const char *fmt, ...);
int vprintspeakf(const char *fmt, va_list args);

int speakf(const char *fmt, ...);
int vspeakf(const char *fmt, va_list args);

#endif
