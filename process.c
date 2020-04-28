#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/syscall.h>

#include "process.h"
#include "util.h"

#define printk(msg, ...) syscall(333, msg, __VA_ARGS__)
#define getnstimeofday(ts) syscall(334, ts)

int assignToCPU(const int PID, const int coreIndex) {
	if (coreIndex > sizeof(cpu_set_t)) {
		fprintf(stderr, "[assignToCPU]: core index out of bound!\n");
		return -1;
	}

	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(coreIndex, &mask);
	
	if (sched_setaffinity(PID, sizeof(mask), &mask) < 0) {
		fprintf(stderr, "[assignTOCPU]: set CPU affinity error!\n");
		return -1;
	}

	return 0;
}

int processWake(const int PID) {
	struct sched_param sp;
	sp.sched_priority = 99;

	int ret = 0;
	if ((ret = sched_setscheduler(PID, SCHED_FIFO, &sp)) < 0) {
		perror("[processWake]: process wake error!");
		return -1;
	}
	return ret;
}

int processSleep(const int PID) {
	struct sched_param sp;
	sp.sched_priority = 1;

	int ret = 0;
	if ((ret = sched_setscheduler(PID, SCHED_FIFO, &sp)) < 0) {
		fprintf(stderr, "[processSleep]: process sleep error!\n");
		return -1;
	}
	return ret;
}

// fork and execute a new process
int processExecute(Process *process, const int warmup) {
	int pid = fork();

	if (pid < 0) {
		fprintf(stderr, "[processExecute]: fork error!\n");
		return -1;
	} else if (pid > 0) {
		process->PID = pid;
		assignToCPU(pid, CHILD_CPU);
		processSleep(pid);
		return pid;
	} else {
		for (int i = 0; i < 10*warmup; i++) // To prevent child from getting "start" to early
			unitTime();

		struct timespec start = {0, 0}, end = {0, 0};
		getnstimeofday(&start);
		if (start.tv_sec == 0 || start.tv_nsec == 0) {
			fprintf(stderr, "[processExecute]: getnstimeofday(start) error!\n");
			exit(1);
		}

		for (int t = 0; t < process->time; t++)
			unitTime();

		getnstimeofday(&end);
		if (end.tv_sec == 0 || end.tv_nsec == 0) {
			fprintf(stderr, "[processExecute]: getnstimeofday(end) error!\n");
			exit(1);
		}
		printk("[Project1] %d %ld.%09ld %ld.%09ld\n", getpid(), start.tv_sec, start.tv_nsec, end.tv_sec, end.tv_nsec);
		exit(0);
	}
}