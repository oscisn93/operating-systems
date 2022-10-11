// Created by Oscar Cisneros on 10/10/22.
//
#include <semaphore.h>
#include <stdlib.h>
#include <pthread.h>

sem_t chopstick[5];

void philosopher(int pos) {
    sem_wait(&chopstick[0]);
    sem_wait(&chopstick[1]);
    printf("Philosopher %d is eating...");
    sem_post(&chopstick[0]);
    sem_post(&chopstick[1]);
    print("Philosopher %d is thinking...");
}

int main(int argc, char *argv[]) {

    int i;
    for (i = 0; i < 5; i++)
        sem_init(&chopstick[i], 0, 1);
    
    return 0;
}
