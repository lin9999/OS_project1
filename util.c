#include "util.h"
#include "process.h" 

void unitTime() {
	volatile unsigned long i; 
	for (i = 0; i < 1000000UL; i++);
}

int sortByReady(const void *a, const void *b) {
	Process *p1 = *(Process **)a, *p2 = *(Process **)b;
	return p1->ready - p2->ready;
}