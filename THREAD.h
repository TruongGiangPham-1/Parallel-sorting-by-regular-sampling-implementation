#ifndef THREAD_

#define THREAD_
#include <pthread.h>
typedef struct THREAD{
    // info about the thread
    int threadIndex;
    pthread_t threadID;
    int startIdx;  // start index of the array they are in charge of
    int endIdx;   // end index of the arrya they are in cahrage of
    //
    // phase 1 info
    int localSampleLen; // length of local sample array

    // phase 3 info;
    long int**partitionIndices;  // dim=(P, 2) matrix of [[start, end], [start2, end2], [start3, end3]]. the start end pos of each partition
    // partitionIndices[i, :] = [start, end] range given to this THREAD by process i
    // if threadIndex == i, then 

    int partitionSize;
} THREAD;

#endif // !
