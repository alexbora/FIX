/**
 * @author      : alex (alexbora@gmail.com)
 * @file        : libs
 * @created     : Thursday Feb 25, 2021 17:27:16 EET
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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


