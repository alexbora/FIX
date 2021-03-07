/**
 * @author      : alex (alexbora@gmail.com)
 * @file        : single_order
 * @created     : Thursday Feb 25, 2021 09:34:37 EET
 */

#ifndef SINGLE_ORDER_H

#define SINGLE_ORDER_H
#include <openssl/ssl.h>
typedef struct dictionary dictionary;
typedef struct keyval keyval;

char *itoa(int, char *, int);
void format_time(char *);
char *csum(char *, char *);
keyval *keyval_new(char *, void *);
dictionary *dictionary_new();
void dictionary_add(dictionary *, keyval *);
void message_new2(dictionary *, char **, char *);
int st();
char *init(char *);
#endif /* end of include guard SINGLE_ORDER_H */

