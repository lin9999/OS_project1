#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>

#include "util.h"
#include "process.h"
#include "schedule.h"

static int timer = 0;
static int runningIndex = -1;
static int RR_last_switch = 0;
static int nFinish = 0;

int getNextIndex(const Policy policy, const unsigned nProcess, Process **processList) {
	int next = -1;

	// non-preemptive policy
	if (runningIndex != -1 && (policy == FIFO || policy == SJF))
		return runningIndex;

	switch (policy) {
		case SJF:
		case PSJF:
			for (int i = 0; i < nProcess; i++) {
				if (processList[i]->PID == -1 || processList[i]->time == 0)
					continue;
				if (next == -1 || processList[i]->time < processList[next]->time)
					next = i;
			}

			break;
		
		case FIFO:
			for (int i = 0; i < nProcess; i++) {
				if (processList[i]->PID == -1 || processList[i]->time == 0)
					continue;
				if (next == -1 || processList[i]->time != 0) {
					next = i;
					break;
				}
			}			
			break;
		
		case RR:
			if (runningIndex == -1) { // find any available process
				for (int i = 0; i < nProcess; i++) {
					if (processList[i]->PID != -1 && processList[i]->time > 0) {
						next = i;
						break;
					}
				}
			} else if ((timer - RR_last_switch) % 500 == 0) { // context switch
#ifdef SHOWMSG
				fprintf(stdout, "[scheduler]: context switch\n");
#endif
				next = (runningIndex + 1) % nProcess;
				while (processList[next]->PID == -1 || processList[next]->time == 0)
					next = (next + 1) % nProcess;
			} else { // no other process can run
				next = runningIndex;
			}
			break;

		default:
			fprintf(stderr, "unknown policy!\n");
	};

#ifdef SHOWMSG
	fprintf(stdout, "[getNextIndex]: nextIndex=%d\n", next);
#endif

	return next;
}

int scheduler(const Policy policy, const unsigned nProcess, Process **processList) {
	qsort(processList, nProcess, sizeof(Process *), sortByReady);

	assignToCPU(getpid(), SCHED_CPU);

	// set scheduler to the highest priority, scheduler should be always running
	processWake(getpid());

	while (1) {
#ifdef SHOWMSG
		fprintf(stdout, "[scheduler]: time = %d\n", timer);
		if (runningIndex != -1)
			fprintf(stdout, "[scheduler]: %s is running\n", processList[runningIndex]->name);
#endif

		//child process has finished
		if (runningIndex != -1 && processList[runningIndex]->time == 0) {
#ifdef SHOWMSG
			fprintf(stdout, "[scheduler]: %s has finished\n", processList[runningIndex]->name);
#endif
			waitpid(processList[runningIndex]->PID, NULL, 0);
			runningIndex = -1;
			nFinish++;
			if (nFinish == nProcess)
				break;
		}

		// someone is ready, start a new process
		for (int i = 0; i < nProcess; i++) {
			if (timer == processList[i]->ready) {
				int warmup = 0;
				if (policy == FIFO)
					warmup = i;
				else if (policy == RR)
					warmup = 5;
				processExecute(processList[i], warmup);
			}
		}

		int nextIndex = getNextIndex(policy, nProcess, processList);

		// context switch
		if (nextIndex != -1 && nextIndex != runningIndex) {
			if (runningIndex != -1) {
#ifdef SHOWMSG
				fprintf(stdout, "[scheduler]: timer => %d, sleep: %d\n", timer, processList[runningIndex]->PID);
#endif
				processSleep(processList[runningIndex]->PID);
			}
#ifdef SHOWMSG
				fprintf(stdout, "[scheduler]: timer => %d, wake: %d\n", timer, processList[nextIndex]->PID);
#endif
			processWake(processList[nextIndex]->PID);
			runningIndex = nextIndex;
			RR_last_switch = timer;
		}

		unitTime();
		timer++;
		if (runningIndex != -1)
			processList[runningIndex]->time--;
	}
	
	for (int i = 0; i < nProcess; i++)
		fprintf(stdout, "%s %d\n", processList[i]->name, processList[i]->PID);

	return 0;
}

