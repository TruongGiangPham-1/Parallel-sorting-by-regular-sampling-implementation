#include <algorithm>
#include <cassert>
#include <climits>
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
#include <sys/time.h>		/* needed for the gettimeofday() system call */


using namespace std;


#define BEGIN 0
#define END 1

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
long int* sampleArray = nullptr;  // the global regular sample array
long int* pivots = nullptr;

unsigned long** times = nullptr;

vector<THREAD*> threadList;

// comparator for qsort()
int cmpfunc (const void * a, const void * b) {
   return ( *(long int*)a - *(long int*)b );
}

void* ThreadFunc(void* arg) {
    THREAD* threadConfig = ((THREAD*) arg);
    
    //printf("in threadindex %d, start=%d, end=%d\n", threadConfig->threadIndex, threadConfig->startIdx, threadConfig->endIdx);

    phase1(threadConfig->threadIndex, threadConfig->startIdx, threadConfig->endIdx);
    pthread_barrier_wait(&barrier);  
    // only proceeds if P number of threads call pthread_barrier_wait()
    //
    //
    phase2(threadConfig->threadIndex);
    pthread_barrier_wait(&barrier);


    phase3(threadConfig->threadIndex);
    pthread_barrier_wait(&barrier);

    //printPartition(threadConfig->threadIndex);

    phase4(threadConfig->threadIndex);
    pthread_barrier_wait(&barrier);
    
    // here all thread have moved their partitions


    for (int i = 0; i < P; i++) {
        delete[] ((THREAD*)arg)->partitionIndices[i];
    }
    delete[] ((THREAD*)arg)->partitionIndices;
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


    // allocate (P, 2) mattrix
    tptr->partitionIndices = new long int*[P]; 
    for (int i = 0; i < P; i++) {
        tptr->partitionIndices[i] = new long int[2];
        tptr->partitionIndices[i][0] = -1;
        tptr->partitionIndices[i][1] = -1;
    }
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
        threadList.push_back(tptr);
        if (pthread_create(&th[i], NULL, &ThreadFunc, tptr) != 0) {
            perror("pthread create failed");
            exit(1);
        }
    }

    THREAD* tptr = allocateTHREAD(th[P - 1], P - 1, startBound);
    tptr->endIdx = N - 1;
    threadList.push_back(tptr);
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


// https://people.cs.rutgers.edu/~pxk/416/notes/c-tutorials/times.html
void phase1(int tindex, int startIdx, int endIdx) {
    // need start, end index of the array that it is in charge of
    // queck sort
    // qsort(base addr, num element, size, cmp)

    struct timeval start;	/* starting time */
	struct timeval end;	/* ending time */
	unsigned long e_usec;	/* elapsed microseconds */

    gettimeofday(&start, 0);

    qsort(arrPtr + startIdx, endIdx - startIdx + 1, sizeof(long int),  cmpfunc);
    isSorted(arrPtr, startIdx,  endIdx);

    // regular sample index 0, w, 2w, ... , (p - 1)w      [total of s=p samples]
    int idx0 = startIdx;
    int w = N/pow(P, 2);
    int idxEnd = (P - 1)* w;
    int regularSampleSize = endIdx - startIdx;   // upper bound for sample size is N/P
    
    //(threadList[tindex])->localSample = new long int[regularSampleSize];
    
     
     
    int sampleSize = 0;
    for (int i = idx0; i < endIdx; i += (w)) {
        long int sample = arrPtr[i];
        //(threadList[tindex])->localSample[sampleSize]  = sample;
        (threadList[tindex])->localSampleLen = sampleSize + 1;


        sampleArray[(tindex*P) + sampleSize] = sample;  // inplace sapmle array

        sampleSize ++;
        if (i == i + w) {  // case: when w == 0, happens when N/P^2 = 0
            break;
        }
    }


    //printPhase1Samples(tindex, (threadList[tindex])->localSample, (threadList[tindex])->localSampleLen);
    gettimeofday(&end, 0);
    e_usec = ((end.tv_sec * 1000000) + end.tv_usec) - ((start.tv_sec * 1000000) + start.tv_usec);
    times[tindex][0] = e_usec;
}

void phase2(int tindex) {
    if (tindex == 0) {
        struct timeval start;	/* starting time */
        struct timeval end;	/* ending time */
        unsigned long e_usec;	/* elapsed microseconds */

        gettimeofday(&start, 0);
        

        //printGlobalSamples();
        // sort and sample the pivots
        qsort(sampleArray, P*P, sizeof(long int), cmpfunc);
        isSorted(sampleArray, 0, P*P - 1);

        // sample pivots (p + p/2, 2p + p/2, 3p + p/2, ...., (p-1)p + p/2)  total of P - 1 pivots
        pivots = new long int[P - 1];

        int pho = P/2;
        for (int i = 0; i < P - 1; i++) {
            pivots[i] = sampleArray[(i + 1)*(P) + pho - 1];
        }
        //printPivots();
        //
        gettimeofday(&end, 0);
        
        e_usec = ((end.tv_sec * 1000000) + end.tv_usec) - ((start.tv_sec * 1000000) + start.tv_usec);
        times[tindex][1] = e_usec;
    }
}

void phase3(int tindex) {
    struct timeval start;	/* starting time */
    struct timeval end;	/* ending time */
    unsigned long e_usec;	/* elapsed microseconds */

    gettimeofday(&start, 0);




    // compute the index range of the partitions for other Processes
    THREAD* t = threadList[tindex];
    int startIdx = t->startIdx;
    int endIdx = t->endIdx;

    int partition = 0;  // pivots used == threadIndex the current partition belongs to
    int pIdx = 0;  // pivot used


    int currParitionBegin = startIdx;
    int currPartitionEnd = -1;
    for (int i = startIdx + 1 ; i <= endIdx; i++) {
        if (arrPtr[i] > pivots[pIdx] && arrPtr[i - 1] <= pivots[pIdx]) {
            currPartitionEnd = i - 1;
            // send this partition to foreignThread
            threadList[partition]->partitionIndices[tindex][0] = currParitionBegin; 
            threadList[partition]->partitionIndices[tindex][1] = currPartitionEnd; 
            
            pIdx ++;
            partition ++;
            currParitionBegin = i;  // begin at next partition
        }
    }
    // send the last partition
    assert(partition == P - 1);  // last partition

    threadList[partition]->partitionIndices[tindex][0] = currParitionBegin; 
    threadList[partition]->partitionIndices[tindex][1] = endIdx; 



    gettimeofday(&end, 0);
    e_usec = ((end.tv_sec * 1000000) + end.tv_usec) - ((start.tv_sec * 1000000) + start.tv_usec);
    times[tindex][2] = e_usec;
}

void phase4(int tindex) {
    struct timeval start;	/* starting time */
    struct timeval end;	/* ending time */
    unsigned long e_usec;	/* elapsed microseconds */

    gettimeofday(&start, 0);

    // k ways merge sort
    // naive running time is O(KN) k = number of paritions = P doesnt matter if we use heap to k way merge O(log(k) N) since K is snmall
    //
    THREAD* t = threadList[tindex];

    int elementLeft = 0;
    for (int i = 0; i < P; i++) {
        if (t->partitionIndices[i][END] >=0 && t->partitionIndices[i][BEGIN] >= 0) {
            elementLeft += (t->partitionIndices[i][END] - t->partitionIndices[i][BEGIN]) + 1;
        }
    }

    int elementTotal = elementLeft;
    t->partitionSize = elementTotal;

    int idx = 0;
    long int* localFinalArray = new long int[elementTotal];  // local sorted array
    while (elementLeft > 0) {
        long int min = getMinHead(t);

        localFinalArray[idx] = min;
        idx ++;
        elementLeft --;
    }
    assert(getMinHead(t) == -1);  // no more element so this should return -1
    //printf("printint sorted partition THREAD %d\n", t->threadIndex);
    //printArray(localFinalArray, 0, elementTotal - 1);

    pthread_barrier_wait(&barrier);


    mergeToBig(localFinalArray, elementTotal, t);


    
    gettimeofday(&end, 0);
    e_usec = ((end.tv_sec * 1000000) + end.tv_usec) - ((start.tv_sec * 1000000) + start.tv_usec);
    times[tindex][3] = e_usec;
    

    delete[] localFinalArray;
    return;
}

// return min head from k list and increment the head pointer of those found, return -1 if all k list are used
long int getMinHead(THREAD* t) {
    // out of k arrays, return the one with smallest

    long int min = LONG_LONG_MAX;
    long int config[2] = {-1, -1};  // {headement, whichpartition}

    for (int i = 0; i < P; i++) {

        int idx = t->partitionIndices[i][BEGIN];
        // we are done with this partition
        if (t->partitionIndices[i][BEGIN] > t->partitionIndices[i][END]) {
            continue;
        }

        if (min > arrPtr[idx]) {
            min = arrPtr[idx];
            config[0] = min;
            config[1] = i;
        }
    }
    
    if (config[0] >= 0) {
        // there was a valid min
        int whichP = config[1];
        t->partitionIndices[whichP][0] ++;  // move up the head pointer for this partition
        
    }
    return config[0];  // return the minium

}

void mergeToBig(long int * a, int size, THREAD *t) {
    int startIdx = 0;

    for (int i = 0; i < t->threadIndex; i++) {
        startIdx += threadList[i]->partitionSize; 
    }
    //printf("mergeing thread %d startpost is %d\n", t->threadIndex, startIdx);

    for (int i = startIdx; i < startIdx + size; i++) {
        arrPtr[i] = a[i - startIdx];
    } 
    return;
}

// generate and populate array of size N
void generateData() {
    arrPtr = new long int[N];
    sampleArray = new long int[P*P];  // generate s*p samples, assume that s == p
    srandom(time(nullptr));
    for (int i = 0; i < N; i++) {
        //array.push_back(random());
        arrPtr[i] = random();
    }
}
//
void generateDatahardCode() {
    N = 36;
    P = 3;
    arrPtr = new long int[N];
    sampleArray = new long int[P*P];  // generate s*p samples, assume that s == p
    int arr[36] = {16, 2, 17, 24, 33, 28, 30, 1, 0, 27, 9, 25
           ,34, 23, 19, 18, 11, 7, 21, 13, 8, 35, 12, 29, 
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
        //cout << arrPtr[i] << " ";
        printf("%ld ", arrPtr[i]);
    }
    printf("\n");
}

void printArray(long int* arr, int begin, int end) {
    for (int i = begin; i <= end; i++) {
        //cout << arr[i] << " ";
        printf("%ld ", (long int)arr[i]);
    }
    printf("\n");
}

void printPivots() {
    printf("print pivots\n");
    for (int i = 0; i < P - 1; i++) {
        cout << pivots[i] << " ";
    }
    cout << "\n";
}

void printGlobalSamples() {
    printf("global samples array from phase 1\n");
    for (int i = 0; i < P*P; i++) {
        cout << sampleArray[i] << " ";
    }
    cout << "\n";
}

void printPartition(int tindex) {
    THREAD* t = threadList[tindex];
    printf("printing partition for thread %d\n", tindex);
    for (int i = 0; i < P; i++) {
        // p partitions
        int begin = t->partitionIndices[i][0];
        int end = t->partitionIndices[i][1];
        printArray(arrPtr, begin, end);
        //printf("begin: %d  end: %d\n", begin, end);
    }
}


int isSorted(long int* arr, int start, int end)  {
    for (int i = start + 1; i <= end; i++) {
        assert(arr[i] >= arr[i - 1]);
    }
    return 0;
}


void printTime(unsigned long ** times) {
    unsigned long p1Time = 0;
    for (int i = 0; i < P; i++) {
        p1Time += times[i][0];
    }
    printf("Phase1 time is %lu\n", p1Time / P);

    unsigned long p2Time = times[0][1];
    printf("Phase2 time is %lu\n", times[0][1]);

    unsigned long p3Time = 0;
    for (int i = 0; i < P; i++) {
        p3Time += times[i][2];
    }
    printf("Phase3 time is %lu\n", p3Time / P);

    unsigned long p4Time = 0;
    for (int i = 0; i < P; i++) {
        p4Time += times[i][3];
    }
    printf("Phase4 time is %lu\n", p4Time / P);


    printf("total time is %lu\n", p1Time/P + p2Time + p3Time/P + p4Time/P);
}

// create main function that takes in n and p as input
/*
*   ./a.out n p
*
*/
int main(int argc, char* argv[]) {
    
    //printf("%d\n", argc);
    assert(argc == 3);
    //printf("argument n=%d, p=%d\n", atoi(argv[1]), atoi(argv[2]));
    
    
    N = atoi(argv[1]);
    P = atoi(argv[2]);
    
    printf("%d CORES\n", P);
    times = new unsigned long*[P];  // times[P][0] = phase 1 time of P=1, 
    for (int i = 0; i < P; i++) {
        times[i] = new unsigned long[4];
    }

    generateData();
    //generateDatahardCode();

    //printf("array before sort\n");
    //printArray(arrPtr, 0, N - 1);
    psrs();
    //printf("array after sort\n");
    //printArray(finalArray, 0, N - 1);
    //isSorted(arrPtr, 0, N - 1);
    //
    //
    printTime(times);

    delete[] arrPtr;
    delete[] sampleArray;
    for (int i = 0; i < P; i++) {
        delete[] times[i];
    }
    delete[] times;

    return 0;
}
