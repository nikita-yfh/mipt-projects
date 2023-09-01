#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>

#include "processRunner.h"
#include "signalHandler.h"

ProcessOutput *createProcessOutput() {
	int protection = PROT_READ | PROT_WRITE;
	int visibility = MAP_SHARED | MAP_ANONYMOUS;
	return (ProcessOutput*) mmap(NULL, sizeof(ProcessOutput), protection, visibility, -1, 0);
}

void deleteProcessOutput(ProcessOutput *output) {
	munmap(output, sizeof(ProcessOutput));
}

void runProcess(int in, int out, const char *path, const char *const *cmd) {
	dup2(out, STDOUT_FILENO);
	dup2(in,  STDIN_FILENO);
	execvp(path, (char*const*) cmd);
	close(out);
	close(in);
}
