/**
 * @author      : alex (alexbora@gmail.com)
 * @file        : banker
 * @created     : Tuesday Mar 02, 2021 20:46:34 EET
 */

#include <stdlib.h>

/* banker_stats.c */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* increase the accounts and threads, but make sure there are
 * "too many" threads so they tend to block each other */
#define N_ACCOUNTS 50
#define N_THREADS 100
#define N_ROUNDS 10000

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

struct account {
  long balance;
  pthread_mutex_t mtx;
} accts[N_ACCOUNTS];

int rand_range(int N) {
  return (int)((double)rand() / ((double)RAND_MAX + 1) * N);
}

/* keep a special mutex and condition variable
 * reserved for just the stats */
pthread_mutex_t stats_mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t stats_cnd = PTHREAD_COND_INITIALIZER;
int stats_curr = 0, stats_best = 0;

/* use this interface to modify the stats */
void stats_change(int delta) {
  pthread_mutex_lock(&stats_mtx);
  stats_curr += delta;
  if (stats_curr > stats_best) {
    stats_best = stats_curr;
    /* signal new high score */
    pthread_cond_broadcast(&stats_cnd);
  }
  pthread_mutex_unlock(&stats_mtx);
}

/* a dedicated thread to update the scoreboard UI */
void *stats_print(void *arg) {
  int prev_best;

  (void)arg;

  /* we never return, nobody needs to
   * pthread_join() with us */
  pthread_detach(pthread_self());

  while (1) {
    pthread_mutex_lock(&stats_mtx);

    prev_best = stats_best;
    /* go to sleep until stats change, and always
     * check that they actually have changed */
    while (prev_best == stats_best) pthread_cond_wait(&stats_cnd, &stats_mtx);

    /* overwrite current line with new score */
    printf("\r%2d", stats_best);
    pthread_mutex_unlock(&stats_mtx);

    fflush(stdout);
  }
}

void *disburse(void *arg) {
  size_t i, from, to;
  long payment;

  (void)arg;

  for (i = 0; i < N_ROUNDS; i++) {
    from = rand_range(N_ACCOUNTS);
    do {
      to = rand_range(N_ACCOUNTS);
    } while (to == from);

    pthread_mutex_lock(&accts[MIN(from, to)].mtx);
    pthread_mutex_lock(&accts[MAX(from, to)].mtx);

    /* notice we still have a lock hierarchy, because
     * we call stats_change() after locking all account
     * mutexes (stats_mtx comes last) */
    stats_change(1); /* another banker in crit sec */
    if (accts[from].balance > 0) {
      payment = 1 + rand_range(accts[from].balance);
      accts[from].balance -= payment;
      accts[to].balance += payment;
    }
    stats_change(-1); /* leaving crit sec */

    pthread_mutex_unlock(&accts[MAX(from, to)].mtx);
    pthread_mutex_unlock(&accts[MIN(from, to)].mtx);
  }
  return NULL;
}

int main(void) {
  size_t i;
  long total;
  pthread_t ts[N_THREADS], stats;

  srand(time(NULL));

  for (i = 0; i < N_ACCOUNTS; i++)
    accts[i] = (struct account){100, PTHREAD_MUTEX_INITIALIZER};

  for (i = 0; i < N_THREADS; i++) pthread_create(&ts[i], NULL, disburse, NULL);

  /* start thread to update the user on how many bankers
   * are in the disburse() critical section at once */
  pthread_create(&stats, NULL, stats_print, NULL);

  for (i = 0; i < N_THREADS; i++) pthread_join(ts[i], NULL);

  /* not joining with the thread running stats_print,
   * we'll let it disappar when main exits */

  for (total = 0, i = 0; i < N_ACCOUNTS; i++) total += accts[i].balance;

  printf("\nTotal money in system: %ld\n", total);
}

