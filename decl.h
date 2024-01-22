#ifndef DECL_H_
#define DECL_H_

// function declaration
#include <vector>
#include <pthread.h>
#include "THREAD.h"

void generateData();

void psrs();
void printArray();
void phase1(int tindex, int startIdx, int endIdx);
void* ThreadFunc(void* arg);

int isSorted(int start, int end); // chekc if array between the range is sorted
THREAD* allocateTHREAD(pthread_t tid, int tindex, int startIdx);

void printPhase1Samples(int tindex, long int* samples, int len);

#endif // !DECL_H_
