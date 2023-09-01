#include <sys/wait.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "processRunner.h"
#include "signalHandler.h"

int main(int argc, const char *const argv[]) {
    int in[2] = {}, out[2] = {};
    pipe(in);
	pipe(out);

	ProcessOutput *output = createProcessOutput();

    pid_t cpid = fork();

    if (cpid == 0) { // child
        close(out[0]);
		close(in[1]);
		bindSignals(output);
		
		const char *const cmd[] = {"cat", NULL};
		runProcess(in[0], out[1], "cat", cmd);
	} else {
        close(out[1]);
		close(in[0]);

		write(in[1], "test", 5);
		close(in[1]);

		char buf = 0;
        while (read(out[0], &buf, 1) > 0)
			putchar(buf);
		putchar('\n');

        close(out[0]);
    }

	deleteProcessOutput(output);
}

