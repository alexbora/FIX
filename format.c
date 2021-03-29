#include "format.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <openssl/ssl.h>


#define log(...) do { printf(__VA_ARGS__); fflush(stdout); } while(0)
#define check0(x, ...) if(x) do { log( __VA_ARGS__); exit(1); } while(0)
#define check1(x, ...) if(!(x)) do { log( __VA_ARGS__); exit(1); } while(0)
typedef enum Status { OK, FAILED, NOT_EXECUTED } STATUS;

typedef struct rand_st {
  int (*checksum)(const char *, const size_t, int);
  char *(*format)(const char *, const char *);
  void (*cleanup)(void);
  int (*status)(void);
} RAND_MET;

const RAND_MET *default_meth = NULL;
RAND_MET *active_meth = NULL;

int che(const char *buf, const size_t len, int seed) {
  for (size_t i = 0; i < len; ++i) {
    seed += *buf++;
  }
  return seed % 256;
}

char *form(const char *key, const char *value) {
  char *result = malloc(strlen(key) + strlen(value) + 2);
  strcat(result, key);
  strcat(result, value);
  return result;
}

void xcleanup(void *ptr) {
  if (ptr) free(ptr), ptr = NULL;
}

int status() { return NOT_EXECUTED; }

int RA_status(void) {
  //  const RA_METHOD *meth = RA_get_rand_method();
  //  if (meth && meth->status) return meth->status();
  return 0;
}


int set_method(const RAND_MET *meth) {
  if(!active_meth) default_meth = meth;
  active_meth = meth;
  return 1;
}

typedef struct {
  int a;
  int b;
} rand_meth;
rand_meth rand_met = {1, 2};

rand_meth *RAND_SSLeay(void) { return (&rand_met); }

const RAND_MET *get_method(void)

{
#define default_RAND_meth NULL
  if (!default_RAND_meth) {
    rand_meth *rand_met = RAND_SSLeay();
  }
  return default_RAND_meth;
}

void cleanup(void) {
  const RAND_MET *meth = get_method();
  // RAND_MET *meth;
  if (meth && meth->cleanup) meth->cleanup();
  // RAND_set_rand_method(NULL);
  free(meth);
  meth = NULL;
}

RAND_MET *init() {
  RAND_MET *out = malloc(sizeof(RAND_MET));
  *out = (RAND_MET){
      .checksum = che, .format = form, .cleanup = cleanup, .status = RA_status};
  if (!out) exit(EXIT_FAILURE);
  return out;
}

typedef struct pair {
  char *key;
  char *value;
  size_t len;
} TAG;

char *process(char *in, char *out, RAND_MET *m) {
  char *result = m->format(in, out);
  int csum = m->checksum(result, strlen(result), 0);
  result = realloc(result, sizeof(result) + sizeof(csum) * 8);
  sprintf(result + strlen(result), "%d", csum);
  return result;
}

typedef struct {
  TAG *tag;
  char *result;
} STRING;

void process2(TAG *in, STRING *out, RAND_MET *m) {
  char *result = m->format(in->key, in->value);
  int len = strlen(result);
  int csum = m->checksum(result, len, 0);
  result = realloc(result, sizeof(result) + 1 + sizeof(csum) * sizeof(char *));
  sprintf(result + strlen(result), "%ld", len);
  strcpy(out->result, result);
}


struct ssl_async_args {
    SSL *s;
    void *buf;
    size_t num;
    enum { READFUNC, WRITEFUNC, OTHERFUNC } type;
    union {
        int (*func_read) (SSL *, void *, size_t, size_t *);
        int (*func_write) (SSL *, const void *, size_t, size_t *);
        int (*func_other) (SSL *);
    } f;
};
#if 0
int ssl_read_internal(SSL *s, void *buf, size_t num, size_t *readbytes)
{
  if ((s->mode & SSL_MODE_ASYNC) && ASYNC_get_current_job() == NULL) {
        struct ssl_async_args args;
        int ret;

        args.s = s;
        args.buf = buf;
        args.num = num;
        args.type = READFUNC;
        args.f.func_read = s->method->ssl_read;

        ret = ssl_start_async_job(s, &args, ssl_io_intern);
        *readbytes = s->asyncrw;
        return ret;
    } else {
        return s->method->ssl_read(s, buf, num, readbytes);
    }
}
#endif

int main(int argc, char const *argv[]) {
  RAND_MET *r = init();
  set_method(r);
  char *a = "a";
  char *b = "b";
  char *c = process(a, b, r);
  printf("char c: %s\n", c);
  // r->cleanup(r);
  //  free(r);
  //  free(c);

  TAG tag1 = {"|35=", ""};
  STRING string = {&tag1, malloc(sizeof(tag1.key) + sizeof(tag1.value) + 2)};
  process2(&tag1, &string, r);
  printf("STRING: %s\n", string.result);

  char *text[] = {"A", "B", NULL};

int x= 0;
check1(x == 1, "poll return error events: %d", x);

  for (char **cpp = text; *cpp; cpp++) printf("%s\n", *cpp);
  for (char **cpp = text; *cpp; cpp++) *cpp = malloc(10);
  for (char **cpp = text; *cpp; cpp++) free(*cpp);
  return 0;
}
