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

        if (t->passengersLeft == 0) {
            printf("Ride has ended\n");
            t->finished = true;
            return (void *) NULL;
        }

        mysem_up(&(t->passenger));
    }
    return (void *) NULL;
}
void *passengerThread(void *args) {
    struct train *t = (struct train *) args;
    mysem_down(&(t->passenger));
    printf("Passenger %ld entering train, %d total in train\n", pthread_self() % 100, t->numPass + 1);
    sleep(TIMESLEEP / 3);
    t->numPass++;
    t->passengersLeft--;

    if (t->passengersLeft == 0) {
        mysem_up(&(t->train));
        return (void *) NULL;
    }

    if (t->numPass == t->maxPass) mysem_up(&(t->train));
    else mysem_up(&(t->passenger));
    return (void *) NULL;
}

int main(int argc, char **argv) {
    struct train t;
    int passengers, i;
    pthread_t pidTrain;
    pthread_t *pidPassengers;

    checkParams(argc, argv);
    t.maxPass = atoi(argv[1]);
    t.numPass = 0;
    t.finished = false;
    
    t.train.valid = false;
    t.passenger.valid = false;
    mysem_init(&(t.train), 0);
    pthread_create(&pidTrain, NULL, trainThread, &t);
    
    scanf("%d", &passengers);
    t.passengersLeft = passengers;
    pidPassengers = (pthread_t *) malloc(passengers * sizeof(pthread_t));
    mysem_init(&(t.passenger), 1);
    for (i = 0; i < passengers; i++) {
        pthread_create(&pidPassengers[i], NULL, passengerThread, &t);
    }

    while(t.finished == false);
    
    return 0;
}