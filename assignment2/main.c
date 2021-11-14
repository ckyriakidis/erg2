#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include "../assignment1/mysem.h"

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
    
    return (n <= 1) ? 0 : 1;
}

void *func(void *args) {
    struct threadStruct *tInput = (struct threadStruct *) args;
    int tmp;
    
    while (mysem_down(tInput->s) && !tInput->mainDone) {
        tmp = tInput->input;
        mysem_up(tInput->wait);
        if (isPrime(tmp)) {
            printf("%d is prime\n", tmp);
        }
        else {
            printf("%d is not prime\n", tmp);
        }
    }
    
    /* Signal that you're done */
    while(! mysem_up(tInput->childDone));
    
    return (void *) NULL;
}

int main(int argc, char *argv[]) {
    pthread_t *pid;
    struct threadStruct tInput;
    int i, numWorkers;
    
    if (argc != 2) {
        fprintf(stderr, "format: ./primeThreads <number of threads>\n");
        exit(-1);
    }
    
    numWorkers = atoi(argv[1]);
    
    if (!numWorkers) exit(-1);
    
    tInput.mainDone = false;
    
    tInput.s = malloc(sizeof(struct mysem_t));
    tInput.s->valid = false;
    
    tInput.wait = malloc(sizeof(struct mysem_t));
    tInput.wait->valid = false;
    
    tInput.childDone = malloc(sizeof(struct mysem_t));
    tInput.childDone->valid = false;
    
    pid = (pthread_t *) malloc(numWorkers * sizeof(pthread_t));
    
    mysem_init(tInput.s, 0);
    mysem_init(tInput.wait, 0);
    mysem_init(tInput.childDone, 0);
    
    
    for (i = 0; i < numWorkers; i++)
        pthread_create(&pid[i], NULL, func, (void *) &tInput);
    
    while(scanf("%d", &tInput.input) != EOF) {
        mysem_up(tInput.s);
        mysem_down(tInput.wait);
    }
    
    /* Signal that you're done */
    tInput.mainDone = true;
    for (i = 0; i < numWorkers; i++)
        while(! mysem_up(tInput.s));
    
    /* Wait for the workers to signal that they're done */
    for (i = 0; i < numWorkers; i++)
        mysem_down(tInput.childDone);
    
    free(tInput.s);
    free(tInput.wait);
    free(tInput.childDone);
    
    return 0;
}