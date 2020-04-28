#ifndef PROCESS_H_
#define PROCESS_H_

typedef struct P {
	int PID;
	char name[1 << 5]; // name 
	unsigned int ready; // ready time
	unsigned int time; // execution time
} Process;

#define SCHED_CPU 0
#define CHILD_CPU 1
int assignToCPU(const int PID, const int coreNum);

int processWake(const int PID);
int processSleep(const int PID);

int processExecute(Process *process, const int warmup);

#endif