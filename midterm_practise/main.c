#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define COUNT 5

typedef enum pstate { THINKING, HUNGRY, EATING } pstate;

sem_t mutex;
sem_t chopsticks[COUNT];
pstate cur_states[COUNT];

void *runner1(void *x) {
  int *i = x;
  int pos = *i;
  while (1) {
    printf("\nPhilosopher %d is thinking...\t");
    sem_wait(&mutex);
    sem_wait(&chopsticks[pos]);
    sem_wait(&chopsticks[(pos + 1) % 5]);
    printf("\nPhilosopher %d is eating...\t", pos);
    sem_post(&chopsticks[(pos + 1) % 5]);
    sem_post(&chopsticks[pos]);
    sem_post(&mutex);
  }
  return NULL;
}

void *runner2(void *x) {
  int *i = x;
  int pos = *i;
  while (1) {
    printf("\nPhilosopher %d is thinking...\t");
    sem_wait(&mutex);
    sem_wait(&chopsticks[(pos + 1) % 5]);
    sem_wait(&chopsticks[pos]);
    printf("\nPhilosopher %d is eating...\t", pos);
    sem_post(&chopsticks[pos]);
    sem_post(&chopsticks[(pos + 1) % 5]);
    sem_post(&mutex);
  }
  return NULL;
}

int main(int argc, char const *argv[]) {
  pthread_t philosophers[COUNT];
  pthread_attr_t attr;

  sem_init(&mutex, 0, 1);
  int i;
  for (i = 0; i < COUNT; i++)
    sem_init(&chopsticks[i], 0, 1);

  pthread_attr_init(&attr);
  // thread action

  for (i = 0; i < COUNT; i++) {
    if (i % 2 == 0)
      pthread_create(&philosophers[i], &attr, runner1, &i);
    else
      pthread_create(&philosophers[i], &attr, runner2, &i);
  }
  // cleanup
  for (i = 0; i < COUNT; i++)
    pthread_join(philosophers[i], NULL);

  for (i = 0; i < COUNT; i++)
    sem_destroy(&chopsticks[i]);
  return 0;
}
