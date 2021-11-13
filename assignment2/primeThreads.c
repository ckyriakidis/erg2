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

struct cock {
    struct mysem_t *s;
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

void *func(void *myComm) {
    struct cock *arr = (int *) myComm;
    
    while (mysem_down(arr->s)) {
            if (isPrime(arr)) {
                printf("%d is prime\n", arr);
            }
            else {
                printf("%d is not prime\n", arr);
            }
            arr[0] = 1;
    }
    
    arr[0] = -2;
    
    return (void *) NULL;
}

int main(int argc, char *argv[]) {
    int i, input, numWorkers, **comm;
    pthread_t *pid;
    
    if (argc != 2) {
        fprintf(stderr, "format: ./primeThreads <number of threads>\n");
        exit(-1);
    }
    
    struct cock **penis = malloc(numWorkers * sizeof(struct cock));
    struct mysem_t *s = malloc(numWorkers * sizeof(struct mysem_t));
    numWorkers = atoi(argv[1]);
    
    if (!numWorkers) exit(-1);
    
    comm = (int *) malloc(numWorkers * sizeof(int *));
    pid = (pthread_t *) malloc(numWorkers * sizeof(pthread_t));
    
    for (i = 0; i < numWorkers; i++) {
        mysem_init(&s[i], 0);
        penis[i]->s = &s[i];
        pthread_create(&pid[i], NULL, func, (void *) &penis[i]);
    }
    
    while(scanf("%d", &input) != EOF) {
        do {
            for (i = 0; i < numWorkers && mysem_up(s[i]) == 0; i++);
        } while (i == numWorkers);
        
        penis[i]->input = input;
        mysem_down(&s[i]);
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