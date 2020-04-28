#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "process.h"
#include "schedule.h"

Process **parseInput(Policy *p, unsigned int *nProcess) {
	char in_policy[1 << 5] = {0}; // scheduling policy
	scanf("%s", in_policy);
	if (!strncmp(in_policy, "FIFO", 4))
		*p = FIFO;
	else if (!strncmp(in_policy, "RR", 2))
		*p = RR;
	else if (!strncmp(in_policy, "SJF", 3))
 		*p = SJF;
	else if (!strncmp(in_policy, "PSJF", 4)) 
		*p = PSJF;
	else {
		fprintf(stderr, "Unknown policy!\n");
		exit(1);
	}
	scanf("%d", nProcess);

	Process **processList= (Process **)malloc(sizeof(Process *) * (*nProcess));
	for (int i = 0; i < *nProcess; i++) {
		Process *newProcess = (Process *)malloc(sizeof(Process));
		newProcess->PID = -1;
		scanf("%s", newProcess->name);
		scanf("%d", &newProcess->ready);
		scanf("%d", &newProcess->time);
		processList[i] = newProcess;
	}

	return processList;
}

int main() {
	Policy policy = NONE;
	unsigned int nProcess = 0;

	Process **processList = parseInput(&policy, &nProcess);

#ifdef SHOWMSG
	for (int i = 0; i < nProcess; i++) {
		fprintf(stdout, "process name : %s\n", processList[i]->name);
		fprintf(stdout, "ready time : %d\n", processList[i]->ready);
		fprintf(stdout, "execution time : %d\n", processList[i]->time);
	}
	fprintf(stdout, "start scheduling\n");
#endif
	
	scheduler(policy, nProcess, processList);

	return 0;
}