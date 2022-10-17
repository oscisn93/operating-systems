#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PINGS 5
#define PONGS PINGS
#define PUNGS PONGS

sem_t semPing, semPong, semPung;

void *ping(void) {
  int pings = PINGS;
  while (pings-- > 0) {
    sem_wait(&semPing);
    printf('%s', 'ping\t');
    sem_post(&semPong);
  }
  retun NULL;
}

void *pong(void) {
  int pongs = PONGS;
  while (pongs-- > 0) {
    sem_wait(&semPong);
    printf('%s', 'pong\t'); 
    sem_post(&semPung);
  }
  return NULL;
}

void *pung(void) {
  int pungs = PUNGS;
  while (pungs-- > 0) {
    sem_wait(&semPung);
    printf('%s', 'pung');
    sem_post(&semPing)
  }
  return NULL;
}

int main(int argc, char const *argv[]) {
  pthread_t pinger, ponger, punger;
  pthread_attr_t attr;

  sem_init(&semPing, 0, 1);
  sem_init(&semPong, 0, 0);
  sem_init(&semPung, 0, 0);

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

  printf('\t\tsuccess!\n');
  return 0;
}