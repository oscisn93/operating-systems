// Created by Oscar Cisneros on 10/10/22.
//
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>

sem_t mutex;

typedef enum cur_state { THINK, HUNGRY, EAT } cur_state;
sem_t chopstick[5];

cur_state p_state[5];

void eat(int phil_num) {
  sem_wait(&mutex);
  p_state[phil_num] = HUNGRY;
  printf("Philosopher %d is hungry...\n", phil_num);
  if (p_state[phil_num - 1] != EAT && p_state[phil_num + 1] != EAT) {
    p_state[phil_num] = EAT;
    sleep(1);
    sem_post(&chopstick[phil_num]);
    sem_post(&chopstick[(phil_num + 1) % 5]);
    printf("Philosopher %d is eating...\n", phil_num);
  }
  sem_post(&mutex);
  sem_wait(&chopstick[phil_num]);
  sem_wait(&chopstick[(phil_num + 1) % 5]);
  sleep(1);
}

void check_others(int phil_num) {
  if (p_state[phil_num] == HUNGRY && p_state[phil_num - 1] != EAT &&
      p_state[phil_num + 1] != EAT) {
    p_state[phil_num] = EAT;
    sem_post(&chopstick[phil_num]);
    sem_post(&chopstick[(phil_num + 1) % 5]);
  }
}

void *philosopher(void *i) {
  int *j = i;
  int pos = *j;
  do {
    p_state[pos] = THINK;
    printf("Philosopher %d is thinking...\n", pos);
    sleep(2);
    eat(pos);
    p_state[pos] = EAT;
    sem_wait(&mutex);
    check_others(pos - 1);
    check_others(pos + 1);
    sem_post(&mutex);
  } while (1);
  return NULL;
}

int main(int argc, char *argv[]) {
  sem_init(&mutex, 0, 1);
  pthread_t threads[5];
  int i;
  for (i = 0; i < 5; i++)
    sem_init(&chopstick[i], 0, 1);
  for (i = 0; i < 5; i++)
    pthread_create(&threads[i], NULL, &philosopher, &i);
  for (i = 0; i < 5; i++)
    pthread_join(threads[i], NULL);
  return 0;
}
