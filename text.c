/**
 * @author      : alex (alexbora@gmail.com)
 * @file        : text
 * @created     : Monday Mar 01, 2021 19:02:22 EET
 */

#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
int x = 0;
pthread_mutex_t m;

struct Account {
  long balance;
  pthread_mutex_t mtx;
} accts[100];

volatile int FLAG = 0;

int rand_range(int N) {
  return (int)((double)rand() / ((double)RAND_MAX + 1) * N);
}

void *foo(void *ptr) {
  pthread_mutex_lock(&accts[0].mtx);
  pthread_mutex_lock(&accts[1].mtx);
  x++;
  printf("%d\n", x);
  pthread_mutex_unlock(&accts[0].mtx);
  pthread_mutex_unlock(&accts[1].mtx);
  return NULL;
}
void *foo2(void *ptr) {
  pthread_mutex_lock(&accts[0].mtx);
  pthread_mutex_lock(&accts[1].mtx);
  x--;
  printf("%d\n", x);
  pthread_mutex_unlock(&accts[0].mtx);
  pthread_mutex_unlock(&accts[1].mtx);
  return NULL;
}
int main(int argc, char *argv[]) {
  srand(time(NULL));
#if 0
  pthread_t t[2];
  pthread_mutex_init(&accts[0].mtx, NULL);
  pthread_mutex_init(&accts[1].mtx, NULL);

  pthread_attr_t a;
  pthread_attr_init(&a);

  struct sched_param param;
  param.sched_priority = 99;
  pthread_attr_setschedparam(&a, &param);
  for (unsigned i = 0; i < 5; i++) {
    pthread_create(&t[0], NULL, foo2, NULL);
    pthread_create(&t[0], &a, foo, NULL);
  }
  for (unsigned i = 0; i < 5; i++) {
    pthread_join(t[0], NULL);
  //  pthread_join(t[1], NULL);
  }
  pthread_mutex_destroy(&m);
  printf("TEST ✓");
#endif
  int to, from;
  for (unsigned i = 0; i < 5; i++) {
    from = rand_range(10);
    do {
      to = rand_range(10);
    } while (from == to);
    printf("%d\t%d\n", from, to);
  }
}


