/**
 * @author      : alex (alexbora@gmail.com)
 * @file        : single_order
 * @created     : Thursday Feb 25, 2021 09:33:58 EET
 */

//#include "single_order.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
  char *key;
  char *value;
} keyval;

typedef struct dictionary {
  keyval **pairs;
  int length;
} dictionary;

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

void format_time(char *buf) {
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

keyval *keyval_new(char *key, void *value) {
  keyval *out = malloc(sizeof(keyval));
  *out = (keyval){.key = key, .value = value};
  return out;
}

dictionary *dictionary_new() {
  dictionary *out = malloc(sizeof(dictionary));
  *out = (dictionary){};
  return out;
}

void dictionary_add(dictionary *in, keyval *k) {
  in->length++;
  in->pairs = realloc(in->pairs, sizeof(keyval) * in->length);
  in->pairs[in->length - 1] = k;
}

void message_new2(dictionary *in, char **body) {
  char final[1024] = "";
  //  char tmp[256] = "";
  format_time(in->pairs[3]->value);
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

  // for (int i = 0; i < 8; i++){
  // strcat(tmp, *(body+i));
  //}

  for (char **cpp = body + 1; *cpp; cpp++) {
    strcat(final, cpp[0]);
  }
  // strcat(final, tmp);

  in->pairs[4]->value = checkSum(final, strlen(final));
  strcat(final, in->pairs[4]->key);
  strcat(final, in->pairs[4]->value);
  printf("array: %s\n", final);
}
void message_new(dictionary *in) {
  char body[128] = "";
  // char *body = in->pairs[1]->value;
  format_time(in->pairs[5]->value);
  size_t len = 0;
  for (int i = 1; i < in->length - 1; i++) {
    len += strlen(in->pairs[i]->key) + strlen(in->pairs[i]->value);
  }

  itoa(len, in->pairs[1]->value, 10);
  in->pairs[2]->value = "D";
  in->pairs[14]->value = checkSum(body, strlen(body));

  int j = 0;
  for (int i = 0; i < in->length - 1; i++, j++) {
    strcat(body, in->pairs[j]->key);
    strcat(body, in->pairs[j]->value);
  }
  in->pairs[14]->value = checkSum(body, strlen(body));
  strcat(body, in->pairs[14]->key);
  strcat(body, in->pairs[14]->value);
  // printf("BODY: %s\n", body);
}

int main() {
  dictionary *logout = dictionary_new();
  dictionary_add(logout, keyval_new("8=", "FIX.4.2"));
  dictionary_add(logout, keyval_new("|9=", malloc(64)));
  dictionary_add(logout, keyval_new("|35=", ""));
  dictionary_add(logout, keyval_new("|34=", "1"));
  dictionary_add(logout, keyval_new("|49=", "Coinbase"));
  dictionary_add(logout, keyval_new("|52=", malloc(64)));
  dictionary_add(logout, keyval_new("|56=", "Coinbase"));
  dictionary_add(logout, keyval_new("|15=", "EUR"));
  dictionary_add(logout, keyval_new("|21=", "1"));
  dictionary_add(logout, keyval_new("|38=", "QTY"));
  dictionary_add(logout, keyval_new("|40=", "1"));
  dictionary_add(logout, keyval_new("|44=", "price"));
  dictionary_add(logout, keyval_new("|54=", "side"));
  dictionary_add(logout, keyval_new("|55=", "BTC"));
  dictionary_add(logout, keyval_new("|10=", ""));

  dictionary *template = dictionary_new();
  dictionary_add(template, keyval_new("8=", "FIX.4.2"));
  dictionary_add(template, keyval_new("|9=", malloc(64)));
  dictionary_add(template, keyval_new("|35=", ""));
  dictionary_add(template, keyval_new("|52=", malloc(640)));
  dictionary_add(template, keyval_new("|10=", ""));

  dictionary *body = dictionary_new();
  dictionary_add(body, keyval_new("|49=", "Coinbase"));
  dictionary_add(body, keyval_new("|56=", "Coinbase"));
  dictionary_add(body, keyval_new("|15=", "EUR"));
  dictionary_add(body, keyval_new("|21=", "1"));
  dictionary_add(body, keyval_new("|44=", "price"));
  dictionary_add(body, keyval_new("|54=", "side"));
  dictionary_add(body, keyval_new("|38=", "QTY"));
  dictionary_add(body, keyval_new("|55=", "BTC"));

  char *bdy[] = {
      "D",         "|49=Coinbase", "|56=Coinbase", "|15=EUR", "|21=1",
      "|44=price", "|54=side",     "|38=QTY",      "|55=BTC", NULL};

  message_new2(template, bdy);
  char *bff = malloc(64);
  csum("text", bff);
  printf("bff=%s\n", bff);
  printf("✓");
// message_new(logout);
//
#if 0
  time_t start = clock();

  for (int i = 0; i < 1e6; i++) {
  message_new2(template, bdy);
    //message_new(logout);
    // message_new2(template, body);
  }

  printf("%lf\n", (float)(clock() - start) / CLOCKS_PER_SEC);

#endif

  return 0;
}




