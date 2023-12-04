#include "speaker.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int vspeakf(const char *fmt, va_list args) {
	FILE *in = popen("festival --tts --language russian", "w");
	if(!in)
		return -1;
	vfprintf(in, fmt, args);
	pclose(in);
	return 0;
}

int speakf(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ret = vspeakf(fmt, args);
	va_end(args);
	return ret;
}

int vprintspeakf(const char *fmt, va_list args) {
	va_list args2;
	va_copy(args2, args);
	vprintf(fmt, args);
	fflush(stdout);
	vspeakf(fmt, args2);
	return 0;
}

int printspeakf(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ret = vprintspeakf(fmt, args);
	va_end(args);
	return ret;
}
