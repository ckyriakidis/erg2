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
    
    while (mysem_down(tStruct->s)) {
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
    
    return (void *) NULL;
}

int main(int argc, char *argv[]) {
    int i, input, numWorkers;
    pthread_t *pid;
    
    if (argc != 2) {
        fprintf(stderr, "format: ./primeThreads <number of threads>\n");
        exit(-1);
    }
    
    numWorkers = atoi(argv[1]);
    
    if (!numWorkers) exit(-1);
    
    struct threadStruct threadInput;
    threadInput.s = malloc(sizeof(struct mysem_t));
    threadInput.s->valid = false;
    threadInput.wait = malloc(sizeof(struct mysem_t));
    threadInput.wait->valid = false;
    pid = (pthread_t *) malloc(numWorkers * sizeof(pthread_t));
    
    mysem_init(threadInput.s, 0);
    mysem_init(threadInput.wait, 0);
        
    for (i = 0; i < numWorkers; i++)
        pthread_create(&pid[i], NULL, func, (void *) &threadInput);
    
    while(scanf("%d", &input) != EOF) {
        /*do {
            for (i = 0; i < numWorkers && mysem_up(s) == 0; i++);
        } while (i == numWorkers);
        */
        threadInput.input = input;
        mysem_up(threadInput.s);
        
        mysem_down(threadInput.wait);
    }

    sleep(10);
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
    return 0;
}