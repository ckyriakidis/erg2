#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include "../assignment1/mysem.h"

#define N 5

/*
 * 1 not working
 * 0 processing
 * -1 signaled by main process to terminate
 * -2 signaled by worker process that it has terminated
*/

struct threadStruct {
    struct mysem_t *s;
    struct mysem_t *wait;
    struct mysem_t *childDone;
    bool mainDone;
    int input;
};

int isPrime(int n) {
    int i;
    
    for (i = 2; i <= sqrt(n); i++) {
        if (n % i == 0) {
            return 0;
        }
    }
 
    if (n <= 1)
        return 0;
    
    return 1;
}

void *func(void *args) {
    struct threadStruct *tStruct = (struct threadStruct *) args;
    int tmp;
    
    while (! tStruct->mainDone) {
        mysem_down(tStruct->s);
        
        tmp = tStruct->input;
        mysem_up(tStruct->wait);
        if (isPrime(tmp)) {
            printf("%d is prime\n", tmp);
        }
        else {
            printf("%d is not prime\n", tmp);
        }
        /*  */
    }
    
    /*send signal that you're done*/
    mysem_up(tStruct->childDone);
    
    return (void *) NULL;
}

int main(int argc, char *argv[]) {
    int i, numWorkers;
    pthread_t *pid;
    struct threadStruct threadInput;
    
    if (argc != 2) {
        fprintf(stderr, "format: ./primeThreads <number of threads>\n");
        exit(-1);
    }
    
    numWorkers = atoi(argv[1]);
    
    if (!numWorkers) exit(-1);
    
    threadInput.mainDone = false;
    
    threadInput.s = malloc(sizeof(struct mysem_t));
    threadInput.s->valid = false;
    
    threadInput.wait = malloc(sizeof(struct mysem_t));
    threadInput.wait->valid = false;
    
    threadInput.childDone = malloc(sizeof(struct mysem_t));
    threadInput.childDone->valid = false;
    
    pid = (pthread_t *) malloc(numWorkers * sizeof(pthread_t));
    
    mysem_init(threadInput.s, 0);
    mysem_init(threadInput.wait, 0);
    mysem_init(threadInput.childDone, 0);
    
    
    for (i = 0; i < numWorkers; i++)
        pthread_create(&pid[i], NULL, func, (void *) &threadInput);
    
    while(scanf("%d", &threadInput.input) != EOF) {
        /*do {
            for (i = 0; i < numWorkers && mysem_up(s) == 0; i++);
        } while (i == numWorkers);
        */
        mysem_up(threadInput.s);
        
        mysem_down(threadInput.wait);
    }
    
    threadInput.mainDone = true;
    
    for (i = 0; i < numWorkers; i++)
        mysem_down(threadInput.childDone);
    
    // sleep(10);
    /*
    do {
        for (i = 0; i < numWorkers && comm[i][0] == 1; i++);
    } while (i != numWorkers);
    
    for (i = 0; i < numWorkers; i++) {
        comm[i][0] = -1;
    }
    
    do {
        for (i = 0; i < numWorkers && comm[i][0] == -2; i++);
    } while (i != numWorkers);
    
    for (i = 0; i < numWorkers; i++) {
        free(comm[i]);
    }
    
    free(pid);
    free(comm);
    */
    free(threadInput.s);
    free(threadInput.wait);
    free(threadInput.childDone);
    
    return 0;
}