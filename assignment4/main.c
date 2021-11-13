#include "../assignment1/mysem.h"
#include <unistd.h>

struct train {
    int numPass;
    int maxPass;
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
        sleep(5);
        printf("Passengers left the train\n");
        sleep(1);
        t->numPass = 0;

        int ret = mysem_up(&(t->passenger));
        if (!ret) {
            printf("train ended frim train\n");
            break;
        }
    }
    return (void *) NULL;
}
void *passengerThread(void *args) {
    struct train *t = (struct train *) args;
    mysem_down(&(t->passenger));
    printf("Passenger n.%ld entering train, %d total\n", pthread_self() % 1000, t->numPass + 1);
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

    checkParams(argc, argv);
    t.maxPass = atoi(argv[1]);
    t.numPass = 0;
    t.finished = false;
    
    t.train.valid = false;
    t.passenger.valid = false;
    mysem_init(&(t.train), 0);
    pthread_create(&pidTrain, NULL, trainThread, &t);
    
    scanf("%d", &passengers);
    pidPassengers = (pthread_t *) malloc(passengers * sizeof(pthread_t));
    mysem_init(&(t.passenger), 1);
    for (i = 0; i < passengers; i++) {
        pthread_create(&pidPassengers[i], NULL, passengerThread, &t);
    }

    while(t.finished == false);
    
    return 0;
}