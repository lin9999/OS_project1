#ifndef SCHEDULE_H_
#define SCHEDULE_H_

#include "process.h"

typedef enum policy {
	NONE, FIFO, RR, SJF, PSJF
} Policy;

int scheduler(const Policy policy, const unsigned nProcess, Process **processList);

#endif