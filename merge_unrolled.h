#ifndef __merge_unrolled_h__
#define __merge_unrolled_h__

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef __STDC_WANT_LIB_EXT1__ 
#define __STDC_WANT_LIB_EXT1__ 1
#endif

#if !defined (SIZEOFPTR)
#define SIZEOFPTR ((int)sizeof(void*))
#endif
#if defined (__x86_64__)
#define PAD_SIZE 16
#else
#define PAD_SIZE SIZEOFPTR
#endif
#define PAD(n) (((n) % PAD_SIZE) ? ((n) + (PAD_SIZE - ((n) % PAD_SIZE))) : (n))
#define PRE PAD(4 + 10 + 2)

#ifndef BUF
#define BUF (PRE + 128)
#endif

#define MSG_SIZE (PRE + 4096)

#ifndef BAR
#define BAR '|'
#endif

#ifndef SOH
#define SOH '\1'
#endif

#include <openssl/evp.h>

typedef struct{
   size_t length;
   size_t len;
   unsigned char* data;
   unsigned char** tmp;
   EVP_MD_CTX *mdctx;
   volatile unsigned checksum;
   struct tm *t;
} message;

typedef struct{
   unsigned char* dec;
   EVP_PKEY *key;
} account;

typedef struct{
   const char *api;
   const char *secret;
   const char *pass;
   size_t len;
} logon;
unsigned checksum(const unsigned char *buffer, const size_t len, unsigned seed);
void generate_msg(const account *a1, message *m, const logon *login);
account *account_new(logon *);
message *message_new2(int const size, const account *a);
message *start_message();
static inline 
void format_time(char *buf);

int sockfd(const char hostname[], const int port);
void *foo_heartbeart(void *ptr);;
SSL *ssl_conn(const int sockfd, const char *hostname);
char *heart();
void login(SSL *s, char *login);
char *GenerateCheckSum(char *buf, long bufLen);
void *login2(void *ctx);
void *foo(void *ptr);

#endif /* !__merge_unrolled_h__ */
