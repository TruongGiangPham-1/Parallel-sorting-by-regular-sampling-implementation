#ifndef DECL_H_
#define DECL_H_

// function declaration
#include <vector>
#include <pthread.h>
#include "THREAD.h"

void generateData();

void psrs();
void phase1(int tindex, int startIdx, int endIdx);
void phase2(int tindex);
void phase3(int tindex);
void* ThreadFunc(void* arg);

int isSorted(long int* arr, int start, int end); // chekc if array between the range is sorted
THREAD* allocateTHREAD(pthread_t tid, int tindex, int startIdx);


void printArray();
void printPivots();
void printGlobalSamples();  // print the sampleArray global array

#endif // !DECL_H_
