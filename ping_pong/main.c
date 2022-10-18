#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PINGS 2
#define PONGS 1
#define PUNGS PONGS

sem_t semPing, semPong, semPung;

void *ping(void *x) {
  int pings = PINGS;
  while (pings-- > 0) {
    sem_wait(&semPing);
    printf("ping\t");
    sem_post(&semPong);
  }
  return NULL;
}

void *pong(void *x) {
  int pongs = PONGS;
  while (pongs-- > 0) {
    sem_wait(&semPong);
    printf("pong\t");
    sem_post(&semPung);
    sem_wait(&semPong);
    printf("pong\t");
    sem_post(&semPing);
  }
  return NULL;
}

void *pung(void *x) {
  int pungs = PUNGS;
  while (pungs-- > 0) {
    sem_wait(&semPung);
    printf("pung\t");
    sem_post(&semPong);
  }
  return NULL;
}

int main(int argc, char const *argv[]) {
  pthread_t pinger, ponger, punger;
  pthread_attr_t attr;

  sem_init(&semPing, 0, 1);
  sem_init(&semPong, 0, 0);
  sem_init(&semPung, 0, 0);
  printf("Program running...\n");

  pthread_attr_init(&attr);
  pthread_create(&pinger, &attr, ping, NULL);
  pthread_create(&ponger, &attr, pong, NULL);
  pthread_create(&punger, &attr, pung, NULL);

  pthread_join(pinger, NULL);
  pthread_join(ponger, NULL);
  pthread_join(punger, NULL);

  sem_destroy(&semPing);
  sem_destroy(&semPong);
  sem_destroy(&semPung);

  printf("\n\t...success!\n");
  return 0;
}
