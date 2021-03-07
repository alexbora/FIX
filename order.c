/**
 * @author      : alex (alexbora@gmail.com)
 * @file        : order
 * @created     : Monday Feb 22, 2021 19:00:19 EET
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NELEMS(x) sizeof(x) / sizeof(x[0])

typedef struct {
  char *key;
  void *value;
} keyval;

typedef struct {
  keyval **pairs;
  size_t length;
} dictionary;

enum Keyval_idx { header, body, body2 };
enum Idx_logout {len, type, sender, target, seq, timestamp, text, csum};

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

unsigned checksum(const unsigned char *buffer, const size_t len,
                  unsigned seed) {
  for (size_t i = 0; i < len; ++i) seed += (unsigned)(*buffer++);
  return seed % 256;
}

char *msg(keyval **in) {
  char timestamp[64], tmp[64];
  printf("%s\n", in[header]->key);
  char *buf = malloc(512);
  format_time(timestamp);
  format_time(in[header]->value);
  size_t length = strlen(in[header]->key) + strlen(in[header]->value) +
                  strlen(in[body]->key) + strlen(in[body2]->key) +
                  strlen(timestamp);

  for (int i = 0; i < 3; i++) {
    memcpy(in[i]->value, "TEST", strlen("TEST"));
  }
  char *bf = malloc(128);
  for (int j, i = 0; i < 3; i++) {
    strcat(bf, in[j]->key);
    strcat(bf, in[j]->value);
    j++;
  }
  printf("STRCAT: %s\n", bf);

  sprintf(buf, "8=FIX.4.2|9=%ld%s%s52=%s112=%s", length, in[header]->key,
          in[body2]->key, timestamp, in[body]->key);

  sprintf(tmp, "10=%d|", checksum((const unsigned char *)buf, strlen(buf), 0));
#if 0
memcpy(tmp, "10=", sizeof("10="));
char f[16];
itoa(checksum(buf, strlen(buf), 0), f, 10);
memcpy(tmp+sizeof("10="), f, sizeof(f));
//strcat(tmp, f);
strcat(tmp, "|");
#endif

  strcat(buf, tmp);

  for (size_t i = 0; i < strlen(buf); i++, (buf[i] == '|') ? buf[i] = '\1' : 0)
    ;

  printf("buf=%s\n", buf);
  return buf;
}

keyval *keyval_new(char *key, void *value) {
  keyval *out = malloc(sizeof(keyval));
  *out = (keyval){.key = key, .value = malloc(500)};
  return out;
}



char *msg_logout(keyval **in) {
 // char *tmp = malloc(1024);
  //strcpy(tmp, "8=FIX.4.2");

//enum Idx_logout {len, type, sender, target, seq, timestamp, text, csum};
format_time(in[timestamp]->value);
in[type]->value = "5";
in[sender]->value = in[target]->value = "Coinbase";
in[seq]->value = "3";
in[text]->value = "Logout acknowledgement";
in[len]->value = NULL;
for (int i = 1; i < 7; i++){
  in[len]->value += strlen(in[i]->key) + strlen(in[i]->value);
}
char *t = malloc(8000);
strcat(t, "8=FIX.4.2");

    void *x = malloc(0);free(x);

for (int j, i = 1; i < 7; i++){
  strcat(t, in[j]->key);
  strcat(t, in[j]->value);
  j++;
}
printf("TMP: %s\n", t);

return t;
}

int main() {
  keyval **pairs = malloc(500);
  keyval **pairs_logout = malloc(500);
  char *tmp[] = {"|35=0", "|52=", " TEST|", "|49=Coinbase|56=Coinbase", NULL};
  char *mess_logout[] = {
      "|9=", "|35=", "|49=", "|56=", "|34=", "|52=", "|58=", "|10="};

  for (int i = 0; i <= sizeof(mess_logout) / sizeof(mess_logout[0]); i++) {
    pairs_logout[i] = keyval_new(mess_logout[i], NULL);
  }

msg_logout(pairs_logout);

  // 8=FIX.4.2^9=84^35=5^49=SellSide^56=BuySide^34=3^52=20190606-09:25:34.329^58=Logout
  // acknowledgement^10=049^

  for (int i = 0; i <= sizeof(tmp) / sizeof(*tmp); i += 1) {
    pairs[i] = keyval_new(tmp[i], NULL);
  }

  for (int i = 0; i <= sizeof(**pairs) / sizeof(*pairs); i += 1) {
    printf("KEY: %s\tVALUE: %s\n", pairs[i]->key, (char *)pairs[i]->value);
  }
  for (keyval **cpp = pairs; *cpp; cpp++) {
    printf("%s\n", cpp[0]->key);
  }
  keyval *k1 = keyval_new("A", NULL);
  keyval *k2 = keyval_new("B", NULL);
  keyval *k3 = keyval_new("C", NULL);

  keyval *p[] = {k1, k2, k3};

  // msg(pairs);
  //msg(p);
  free(k1);
  free(pairs);
  st();
#if 0

  int x = 30;
  char f[30];
  itoa(x, f, 10);
  printf("%s\n", f);
  clock_t start;
  start = clock();

  for (int i = 0; i < 1e6; i += 1) {
    msg(pairs);
  }
  printf("time taken: %lf\n", (float)(clock() - start) / CLOCKS_PER_SEC);
#endif
  return 0;
}

