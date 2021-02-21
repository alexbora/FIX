/**
 * @author      : alex (alexbora@gmail.com)
 * @file        : sock
 * @created     : Sunday Jan 31, 2021 12:41:35 EET
 */

#ifndef SOCK_H

#define SOCK_H

static inline int sockfd(const char hostname[], const int port);
static inline SSL *ssl_conn(const int sockfd, const char *hostname);
static inline void login(SSL *s, char *login);
static inline char *heart();
static inline void format_time(char *buf);

static inline mark_factors(void *ptr);

GenerateCheckSum(char *buf, long bufLen);
#endif /* end of include guard SOCK_H */


