#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#define BUFSIZE 1024

sem_t mutex, semFull, semBuffer;

void *producer_runner(void *x) {
  int rounds = 10;
  while (rounds-- > 0) {
    printf("\nproducing...\t");
    sem_wait(&semBuffer);
    sem_wait(&mutex);
    printf("Added 1 new element!\t");
    sem_post(&mutex);
    sem_post(&semFull);
  }
  return NULL;
}

void *consumer_runner(void *x) {
  int rounds = 5;
  while (rounds-- > 0) {
    printf("\nConsuming...\t");
    sem_wait(&semFull);
    sem_wait(&mutex);
    printf("Consumed the 1st element!\t");
    sem_post(&mutex);
    sem_post(&semBuffer);
  }
  return NULL;
}

int main(int argc, char const *argv[]) {
  pthread_t producer, consumer;
  pthread_attr_t attr;

  sem_init(&mutex, 0, 1);
  sem_init(&semFull, 0, 0);
  sem_init(&semBuffer, 0, BUFSIZE);

  pthread_attr_init(&attr);

  pthread_create(&producer, &attr, producer_runner, NULL);
  pthread_create(&consumer, &attr, consumer_runner, NULL);

  pthread_join(producer, NULL);
  pthread_join(consumer, NULL);

  sem_destroy(&mutex);
  sem_destroy(&semFull);
  sem_destroy(&semBuffer);

  return 0;
}
