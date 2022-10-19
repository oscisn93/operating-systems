#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define ROUNDS 2

sem_t sem1, sem2, sem3;

void *runner1(void *x) {
  int rounds = ROUNDS;
  while (rounds-- > 0) {
    sem_wait(&sem1);
    printf("1\t");
    sem_post(&sem2);
  }
  return NULL;
}

void *runner2(void *x) {
  int rounds = ROUNDS;
  while (rounds-- > 0) {
    sem_wait(&sem2);
    printf("2\t");
    sem_post(&sem3);
    sem_wait(&sem2);
    printf("2\t");
    sem_post(&sem3);
  }
  return NULL;
}

void *runner3(void *x) {
  int rounds = ROUNDS;
  while (rounds-- > 0) {
    sem_wait(&sem3);
    printf("3\t");
    sem_post(&sem2);
    sem_wait(&sem3);
    printf("3\n");
    sem_post(&sem1);
  }
  return NULL;
}

int main(int argc, char const *argv[]) {
  pthread_t thread1, thread2, thread3;
  pthread_attr_t attr;

  sem_init(&sem1, 0, 1);
  sem_init(&sem2, 0, 0);
  sem_init(&sem3, 0, 0);

  pthread_attr_init(&attr);

  pthread_create(&thread1, &attr, runner1, NULL);
  pthread_create(&thread2, &attr, runner2, NULL);
  pthread_create(&thread3, &attr, runner3, NULL);

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  pthread_join(thread3, NULL);

  sem_destroy(&sem1);
  sem_destroy(&sem2);
  sem_destroy(&sem3);

  return 0;
}
