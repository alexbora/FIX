/**
 * @author      : alex (alexbora@gmail.com)
 * @file        : single_order
 * @created     : Thursday Feb 25, 2021 09:33:58 EET
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <gsl/gsl_errno.h>
#include "single_order.h"
#include <pthread.h>
#include <openssl/ssl.h>
#include "utils.h"
struct keyval {
  char *key;
  char *value;
};

struct dictionary {
  keyval **pairs;
  int length;
};
char *itoa(int value, char *result, int base) {
  // check that the base if valid
  if (base < 2 || base > 36) {
    *result = '\0';
    return result;
  }

  char *ptr = result, *ptr1 = result, tmp_char;
  int tmp_value;

  do {
    tmp_value = value;
    value /= base;
    *ptr++ =
        "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxy"
        "z"[35 + (tmp_value - value * base)];
  } while (value);

  // Apply negative sign
  if (tmp_value < 0) *ptr++ = '-';
  *ptr-- = '\0';
  while (ptr1 < ptr) {
    tmp_char = *ptr;
    *ptr-- = *ptr1;
    *ptr1++ = tmp_char;
  }
  return result;
}
static inline
void format(char *buf) {
  time_t rawtime = time(0);
  struct tm *const restrict t = localtime(&rawtime);
  sprintf((char *)buf,
          "%d"
          "%02d"
          "%02d-"
          "%02d:"
          "%02d:"
          "%02d",
          t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min,
          t->tm_sec);
}
char *checkSum(char *buf, long bufLen) {
  static char tmpBuf[4];
  long idx;
  unsigned int cks;

  for (idx = 0L, cks = 0; idx < bufLen; cks += (unsigned int)buf[idx++])
    ;
  sprintf(tmpBuf, "%03d", (unsigned int)(cks % 256));
  return (tmpBuf);
}

char *csum(char *in, char *out) {
  int seed = 0;
  for (int i = 0; i < strlen(in); i++) {
    seed += *in++;
  }
  return itoa(seed % 256, out, 10);
}
static inline
keyval *keyval_new2(char *key, void *value) {
  keyval *out = malloc(sizeof(keyval));
  *out = (keyval){.key = key, .value = value};
  return out;
}
static inline
dictionary *dictionary_new2() {
  dictionary *out = malloc(sizeof(dictionary));
  *out = (dictionary){};
  return out;
}
static inline
void dictionary_add2(dictionary *in, keyval *k) {
  in->length++;
  in->pairs = realloc(in->pairs, sizeof(keyval) * in->length);
  in->pairs[in->length - 1] = k;
}
static inline
void msg(dictionary *in, char **body, char *final) {
  format(in->pairs[3]->value);
  in->pairs[2]->value = body[0];
  size_t len = 0;
  for (char **cpp = body; *cpp; cpp++) {
    len += strlen(cpp[0]);
  }

  for (int i = 1; i < in->length - 1; i++) {
    len += strlen(in->pairs[i]->key) + strlen(in->pairs[i]->value);
  }
  itoa(len, in->pairs[1]->value, 10);

  int j = 0;
  for (int i = 0; i < in->length - 1; i++, j++) {
    strcat(final, in->pairs[j]->key);
    strcat(final, in->pairs[j]->value);
  }

  for (char **cpp = body + 1; *cpp; cpp++) {
    strcat(final, cpp[0]);
  }
  in->pairs[4]->value = checkSum(final, strlen(final));
 
  //csum(final, in->pairs[4]->value);
  strcat(final, in->pairs[4]->key);
  strcat(final, in->pairs[4]->value);
  printf("array: %s\n", final);
}


char *init(char *final) {
  //char *final = malloc(1024);
  dictionary *template = dictionary_new2();
  dictionary_add2(template, keyval_new2("8=", "FIX.4.2"));
  dictionary_add2(template, keyval_new2("|9=", malloc(64)));
  dictionary_add2(template, keyval_new2("|35=", ""));
  dictionary_add2(template, keyval_new2("|52=", malloc(640)));
  dictionary_add2(template, keyval_new2("|10=", ""));

  char *bdy[] = {
      "D",         "|49=Coinbase", "|56=Coinbase", "|15=EUR", "|21=1",
      "|44=price", "|54=side",     "|38=QTY",      "|55=BTC", NULL};
//har fin[1024];
msg(template, bdy, final);
//printf("%s\n", fin);
return final;
}

