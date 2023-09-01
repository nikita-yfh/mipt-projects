#ifndef PROCESS_RUNNER_H
#define PROCESS_RUNNER_H

/**
 * Create shared memory for some processes. You can fork after and use it
 * @param [in]  size           Size of memory
 * @return                     Pointer to memory
 */
struct ProcessOutput {
	int signum;
};

/**
 * Create ProcessOutput in shared memory
 * @return                     Pointer to struct in shared memory
 */
ProcessOutput *createProcessOutput();

/**
 * Delete ProcessOutput from shared memory
 * @param [in]  output         Pointer to struct to delete
 */
void deleteProcessOutput(ProcessOutput *output);

/*
 * Run process with custom input/output
 * @param [in]  in             Descriptor to stdio
 * @param [in]  out            Descriptor to stdout
 * @param [in]  path           Path to executable
 * @param [in]  cmd            Array of argv[], NULL-terminated
 */
void runProcess(int in, int out, const char *path, const char *const *cmd);

#endif
