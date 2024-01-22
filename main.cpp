#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <assert.h>
#include "global.h"
#include "decl.h"
#include <cmath>

#include <vector>

#include <random>
#include <time.h>
#include "THREAD.h"



using namespace std;



// define global
int N = -1;
int P = -1;



//typedef struct THREAD {
//    // info about the thread
//    int threadIndex;
//    pthread_t threadID;
//    int startIdx;  // start index of the array they are in charge of
//    int endIdx;   // end index of the arrya they are in cahrage of
//
//} THREAD;

pthread_barrier_t barrier;
vector<long int> array;  // arrays to be sorted
long int* arrPtr = nullptr;


vector<THREAD> threadList;

// comparator for qsort()
int cmpfunc (const void * a, const void * b) {
   return ( *(long int*)a - *(long int*)b );
}

void* ThreadFunc(void* arg) {
    THREAD* threadConfig = ((THREAD*) arg);
    
    printf("in threadindex %d, start=%d, end=%d\n", threadConfig->threadIndex, threadConfig->startIdx, threadConfig->endIdx);

    phase1(threadConfig->threadIndex, threadConfig->startIdx, threadConfig->endIdx);
    pthread_barrier_wait(&barrier);  
    // only proceeds if P number of threads call pthread_barrier_wait()

    delete ((THREAD*)arg)->localSample;
    delete (THREAD*)arg;
    return (void*)NULL;
}

THREAD* allocateTHREAD(pthread_t tid, int tindex, int startBound) {
    THREAD* tptr = new THREAD;
    tptr->threadIndex = tindex;
    tptr->threadID = tid;
    tptr->startIdx = startBound;
    tptr->endIdx = startBound + N/P - 1;  // -1 here just for 0 base index
    tptr->localSampleLen = 0;
    tptr->localSample = nullptr;
    return tptr;
}

                        
void psrs() {
    
    pthread_t th[P];
    pthread_barrier_init(&barrier, NULL, P);  // p threads exclude mainthread P + 1 for mainthread too but mainthread has to call barrier_wait()
    int startBound = 0;

    // P - 1 thread and last thread gets the left over
    for (int i = 0; i < P - 1; i++) {
        THREAD* tptr = allocateTHREAD(th[i],  i, startBound);
        startBound += (N/P);
        if (pthread_create(&th[i], NULL, &ThreadFunc, tptr) != 0) {
            perror("pthread create failed");
            exit(1);
        }
    }
    THREAD* tptr = allocateTHREAD(th[P - 1], P - 1, startBound);
    tptr->endIdx = N - 1;
    if (pthread_create(&th[P - 1], NULL, &ThreadFunc, tptr) != 0) {
        perror("pthread create failed");
        exit(1);
    }
    // join p threads here
    // mainthread measure time or sumthing
    
    for (int i = 0; i < P; i++) {
        if ( pthread_join(th[i], NULL) != 0) {
            perror("join failed");
            exit(1);
        }
    }

    pthread_barrier_destroy(&barrier);
    return;
}

void phase1(int tindex, int startIdx, int endIdx) {
    // need start, end index of the array that it is in charge of
    // queck sort
    // qsort(base addr, num element, size, cmp)
    qsort(arrPtr + startIdx, endIdx - startIdx + 1, sizeof(long int),  cmpfunc);
    isSorted(startIdx,  endIdx);

    // regular sample index 0, w, 2w, ... , (p - 1)w
    int idx0 = startIdx;
    int w = N/pow(P, 2);
    int idxEnd = (P - 1)* w;
    int regularSampleSize = endIdx - startIdx;   // upper bound for sample size is N/P
    threadList[tindex].localSample = new long int[regularSampleSize];

    int sampleSize = 0;
    for (int i = idx0; i < endIdx; i += (w)) {
        long int sample = arrPtr[i];
        threadList[tindex].localSample[sampleSize]  = sample;
        threadList[tindex].localSampleLen = sampleSize + 1;
        sampleSize ++;
    }
    printPhase1Samples(tindex, threadList[tindex].localSample, threadList[tindex].localSampleLen);
}

void phase2() {
    // need 
}

// generate and populate array of size N
void generateData() {
    arrPtr = new long int[N];
    srandom(time(nullptr));
    for (int i = 0; i < N; i++) {
        array.push_back(random());
        arrPtr[i] = random();
    }
}

void generateDatahardCode() {
    N = 36;
    P = 3;
    arrPtr = new long int[N];
    int arr[36] = {16, 2, 17, 24, 33, 28, 30, 1, 0, 27, 9, 25
           , 34, 23, 19, 18, 11, 7, 21, 13, 8, 35, 12, 29, 
           6, 3, 4, 14, 22, 15, 32, 10, 26, 31, 20, 5};
    for (int i = 0; i < N; i++) {
        arrPtr[i] = arr[i];
    }
}

void printArray() {
    //for (auto &v: array) {
    //    cout << v << " ";
    //}
    for (int i = 0; i < N; i++) {
        cout << arrPtr[i] << " ";
    }
    cout << "\n";
}
void printPhase1Samples(int tindex, long int* samples, int len) {
    printf("threadID %d phase1samples are \n", tindex);
    for (int i = 0; i < len; i++) {
        cout << samples[i] << " ";
    }
    cout << "\n";
}

int isSorted(int start, int end)  {
    for (int i = start + 1; i <= end; i++) {
        assert(arrPtr[i] > arrPtr[i - 1]);
    }
    return 0;
}

// create main function that takes in n and p as input
/*
*   ./a.out n p
*
*/
int main(int argc, char* argv[]) {
    
    printf("%d\n", argc);
    assert(argc == 3);
    printf("argument n=%d, p=%d\n", atoi(argv[1]), atoi(argv[2]));
    
    
    N = atoi(argv[1]);
    P = atoi(argv[2]);
    //generateData();
    generateDatahardCode();

    printf("array before sort\n");
    printArray();
    psrs();
    printf("array after sort\n");
    printArray();
    isSorted(0, N - 1);
    delete[] arrPtr;
    return 0;
}
