#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

unsigned char crc(const char *restrict buf) {
  unsigned char seed = 0;
  while (*buf++) seed += *buf++;
  return seed + 1;
  ;
}
void format_time(char *restrict buf) {
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
void itoax(uint64_t u, char *restrict out) {
  *out = 0;
  do {
    uint64_t n0 = u;
    *((uint64_t *)out) =
        (*((uint64_t *)out) << 8) | (n0 + '0' - (u /= 10) * 10);
  } while (u);
}

enum { LENGTH, TYPE, BODY, TAIL, TIME } id;
char arr[16][16] = {"|9=", "|35=", "", "10=", "|60="};

enum { LOGOUT, SingleOrder } id2;
char arr2[16][16] = {"5", "D"};

char *order = "BTC,  1";

void fn1(char **restrict string, char *restrict buf, int typ) { /* set type */
  strcpy(string[TYPE], arr[TYPE]);
  strcat(string[TYPE], arr2[typ]);
}

void fn2(char **restrict string, char *restrict buf, int typ) { /* set length */
  strcpy(string[typ], arr[typ]);
  int len = strlen(string[TYPE]) + strlen(string[BODY]);
  itoax(len, buf);
  strcat(string[typ], buf);
}

void fn3(char **restrict string, char *buf, int typ) { /* set csum */
  strcpy(string[typ], arr[typ]);
  strcat(string[TYPE], string[BODY]);
  int cs = crc(string[TYPE]);
  itoax(cs, buf);
  strcat(string[typ], buf);
}

void fn4(char **restrict string, char *buf, int typ) { /* format time */
  format_time(buf);
  strcat(string[TYPE], arr[typ]);
  strcat(string[TYPE], buf);
}

void fn5(char **restrict string, char *restrict buf, int typ) { /* order */
  strcat(string[BODY], order);
}

char **init_string(char *protocol) {
  char **string = NULL;
  string = calloc(16, sizeof(char *));
  for (unsigned i = 0; i < 16; i++) string[i] = calloc(64, sizeof(char));
  strcpy(string[0], "8=");
  strcat(string[0], protocol);
  strcat(string[0], "|9=");
  strcpy(string[2], "|35=");
  return string;
}

void set_type_logout(char **string, char *buf) {
  strcat(string[2], arr2[LOGOUT]);
}

void set_type_order(char **string, char *buf) {
  strcat(string[2], arr2[SingleOrder]);
  strcat(string[2], "|11=");
  format_time(buf);
  strcat(string[2], buf);
  strcat(string[2], "|21=3");
  strcat(string[2], "|55=EUR/BTC");
  strcat(string[2], "|54=10");
  strcat(string[2], "|60=");
  strcat(string[2], buf);
  strcat(string[2], "|40=1");
}

void set_type_security(char **string, char *buf) {
  strcat(string[2], "f|");
  strcat(string[2], "BTC");
}

void set_len(char **string, char *buf) {
  itoax(strlen(string[2] + 1), buf);
  strcat(string[0], buf);
}

void set_csum(char **string, char *buf) {
  strcat(string[0], string[1]);
  strcat(string[0], string[2]);
  itoax(crc(string[0]), buf);
  strcat(string[0], "|10=");
  strcat(string[0], buf);
}

enum { L, O, S } e;

void (*const pf[4][4])(char **string, char *buf) = {
    {set_type_logout, set_len, set_csum},
    {set_type_order, set_len, set_csum},
    {set_type_security, set_len, set_csum},
};

char *mai(int op) {
  char **string = NULL;
  string = calloc(16, sizeof(char *));
  for (unsigned i = 0; i < 16; i++) string[i] = calloc(16, sizeof(char));

  char **string2 = init_string("FIX4.2");
  char *b = malloc(64);

for (unsigned i = 0; i < 3; ++i) {
    pf[op][i](string2, b);
  }
  printf("string2 %s\n", string2[0]);

#if 0
  time_t start = clock();

  char *bf = malloc(64);
  for (unsigned i = 0; i < 1e6; i++) {
    fn1(string, NULL, SingleOrder);
    fn2(string, bf, LENGTH);
    fn3(string, bf, TAIL);
    fn4(string, bf, TIME);
    fn5(string, bf, SingleOrder);
  }

  printf("%lf\n", (float)(clock() - start) / CLOCKS_PER_SEC);
  printf("%s\n", string[TYPE]);
  return 0;

  #endif
return string2[0];
}

