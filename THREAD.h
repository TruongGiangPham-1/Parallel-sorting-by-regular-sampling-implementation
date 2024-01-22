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
    long int* localSample;  // sample in phase 2
    int localSampleLen; // length of local sample array
} THREAD;

#endif // !
