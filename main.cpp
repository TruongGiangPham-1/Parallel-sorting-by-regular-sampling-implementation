
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <assert.h>
#include "global.h"
#include "decl.h"

#include <vector>

#include <random>
#include <time.h>


using namespace std;



// define global
int N = -1;
int P = -1;



typedef struct THREAD {
    // info about the thread
    int threadIndex;
    pthread_t threadID;
    int startIdx;  // start index of the array they are in charge of
    int endIdx;   // end index of the arrya they are in cahrage of

} THREAD;

pthread_barrier_t barrier;
vector<long int> array;  // arrays to be sorted
vector<THREAD> threadList;

//

void* ThreadFunc(void* arg) {
    THREAD* threadConfig = ((THREAD*) arg);
    
    printf("in threadindex %d\n", threadConfig->threadIndex);


    phase1(threadConfig->startIdx, threadConfig->endIdx);
    pthread_barrier_wait(&barrier);  
    // only proceeds if P number of threads call pthread_barrier_wait()

    delete (THREAD*)arg;
    return (void*)NULL;
}

void psrs() {
    generateData();
    // spawn p threads here
    
    pthread_t th[P];
    pthread_barrier_init(&barrier, NULL, P);  // p threads exclude mainthread P + 1 for mainthread too but mainthread has to call barrier_wait()
    int startBound = 0;
    for (int i = 0; i < P; i++) {
        THREAD t = {.threadIndex=i, .threadID=th[i], .startIdx=startBound, .endIdx=startBound + N/P};

        THREAD* tptr = new THREAD;
        tptr->threadIndex = i;
        tptr->threadID = th[i];
        tptr->startIdx = startBound;
        tptr->endIdx = startBound + N/P;
        if (pthread_create(&th[i], NULL, &ThreadFunc, tptr) != 0) {
            perror("pthread create failed");
            exit(1);
        }
    }
    // join p threads here
    // mainthread measure time or sumthing
    
    for (int i = 0; i < P; i++) {
        if ( pthread_join(th[i], NULL) != 0) {
            perror("join failed");
            exit(1);
        }
    }
    return;
}


void phase1(int startIdx, int endIdx) {
    // need start, end index of the array that it is in charge of
}
void phase2() {
    // need 
}


// generate and populate array of size N
void generateData() {
    srandom(time(nullptr));
    for (int i = 0; i < N; i++) {
        array.push_back(random());
    }
}

void printArray() {
    for (auto &v: array) {
        cout << v << " ";
    }
    cout << "\n";
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


    psrs();
    printArray();
    return 0;
}
