#include "../assignment1/mysem.h"
#include <unistd.h>

#define TIMESLEEP 3

struct train {
    int numPass;
    int maxPass;
    int passengersLeft;
    bool finished;
    struct mysem_t train;
    struct mysem_t passenger;
};

void checkParams(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: ./main <maxPassengers>\n");
        exit(-1);
    }
    if (atoi(argv[1]) <= 0) {
        fprintf(stderr, "error: maxPassengers must be a positive integer\n");
        exit(-1);
    }
}

void *trainThread(void *args) {
    struct train *t = (struct train *) args;
    while (mysem_down(&(t->train))) {
        
        printf("Train running with %d passengers \n", t->numPass);
        sleep(TIMESLEEP);
        printf("Passengers left the train\n\n");
        sleep(TIMESLEEP / 2);
        t->numPass = 0;

        mysem_up(&(t->passenger));
    }
    return (void *) NULL;
}
void *passengerThread(void *args) {
    struct train *t = (struct train *) args;
    // printf("passenger %ld created and waiting to be called\n", pthread_self() % 10000);
    mysem_down(&(t->passenger));
    printf("Passenger %ld entering train, %d total in train\n", pthread_self() % 10000, t->numPass + 1);
    sleep(TIMESLEEP / 3);
    t->numPass++;

    if (t->numPass == t->maxPass) mysem_up(&(t->train));
    else mysem_up(&(t->passenger));
    return (void *) NULL;
}

int main(int argc, char **argv) {
    struct train t;
    int passengers, i;
    pthread_t pidTrain;
    pthread_t *pidPassengers;
    int totalPassengers = 0;

    checkParams(argc, argv);
    t.maxPass = atoi(argv[1]);
    t.numPass = 0;
    
    t.train.valid = false;
    t.passenger.valid = false;
    mysem_init(&(t.train), 0);
    pthread_create(&pidTrain, NULL, trainThread, &t);
    pidPassengers = malloc(sizeof(pthread_t));
    mysem_init(&(t.passenger), 1);
    
    while(1) {
        scanf("%d", &passengers);
        totalPassengers += passengers;
        pidPassengers = (pthread_t *) realloc(pidPassengers, totalPassengers * sizeof(pthread_t));
        
        for (i = totalPassengers - passengers; i < totalPassengers; i++) {
            pthread_create(&pidPassengers[i], NULL, passengerThread, &t);
        }
    }

    
    return 0;
}