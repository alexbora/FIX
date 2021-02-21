/**
 * @author      : alex (alexbora@gmail.com)
 * @file        : sock
 * @created     : Sunday Jan 31, 2021 13:16:33 EET
 */

#include <allheaders.h>
#include <fcntl.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "merge_unrolled.h"
#define _GNU_SOURCE
#ifndef BUFISZE
#define BUFSIZE LWS_PRE + 64
#endif

#include <netdb.h>
#include <sys/signal.h>
#include <sys/socket.h>
#ifdef WIN32
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#define close closesocket
#else

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#define h_addr h_addr_list[0] /* for backward compatibility */

#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif

#ifndef HOST
#define HOST "fix.gdax.com"
#endif

#ifndef PORT
#define PORT 4198
#endif

typedef struct Msg {
	char *header;
	char *body;
	char *footer;
	char *final;
	SSL *s;
} msg;

typedef struct {
	SSL *s;
	char *login;
} threading;

void *foo_heartbeart(void *ptr);
pthread_mutex_t m1;

int sockfd(const char hostname[], const int port) {
	register int sockfd = 0;

	struct sockaddr_in servAddr = {0};

#ifdef WIN32
	WSADATA wsaData;

	int initwsa = WSAStartup(MAKEWORD(2, 0), &wsaData);

	if (initwsa != 0) {
		printf("WSAStartup failed: %d\n", initwsa);

		return 1;
	}
#endif

	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	memset(&servAddr, 0, sizeof(servAddr));

	servAddr.sin_family = AF_INET;

	servAddr.sin_port = htons(port);

	struct hostent *host = NULL;

	host = gethostbyname(hostname);

	servAddr.sin_addr.s_addr = *(long *)(host->h_addr);

	memcpy(&servAddr.sin_addr.s_addr, host->h_addr, host->h_length);
	setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (int[]){1}, sizeof(int));
	setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (int[]){1}, sizeof(int));
	setsockopt(sockfd, SOL_SOCKET, SO_SNDLOWAT, (int[]){2}, sizeof(int));

	connect(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr));

#ifdef WIN32
	WSACleanup();

#endif

	return sockfd;
}

SSL *ssl_conn(const int sockfd, const char *hostname) {
	SSL *ssl = NULL;
	SSL_library_init();
	const SSL_METHOD *method = TLS_client_method();
	SSL_CTX *ctx = SSL_CTX_new(method);
	ssl = SSL_new(ctx);
	SSL_set_fd(ssl, sockfd);
	SSL_set_tlsext_host_name(ssl, hostname);
	SSL_connect(ssl);
	return ssl;
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
		t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour,
		t->tm_min, t->tm_sec);
}

char *heart() {
	char *heartbeat = malloc(256);

	if (heartbeat == NULL) {
		fprintf(stderr,
			"\ndynamic memory allocation failed for generating "
			"heartbeat\n");

		exit(EXIT_FAILURE);
	}

	char timestamp[64], tmp[64];

	format_time(timestamp);

	size_t length = strlen("35=0|") + strlen("TEST|") +
			strlen("49=Coinbase|56=Coinbase|") + strlen(timestamp);

	sprintf(heartbeat,
		"8=FIX.4.2|9=%ld|35=0|49=Coinbase|56=Coinbase|52=%s|112=TEST|",
		length, timestamp);

	int chk = checksum(heartbeat, strlen(heartbeat), 0);

	sprintf(tmp, "10=%d|", chk);

	strcat(heartbeat, tmp);

	for (size_t i = 0; i < strlen(heartbeat); i++)
		(heartbeat[i] == '|') ? heartbeat[i] = '\1' : 0;

	return heartbeat;
}

void login(SSL *s, char *login) {
	char *heart_bt = heart();

	time_t t = time(NULL);
	struct tm *tm = localtime(&t);

	char *buf = malloc(MSG_SIZE);
	if (!buf) {
		fprintf(stderr, "%s\n", "malloc-ing receiving buffer failed");
		goto err;
	}
	printf(KGRN "\n Login sent at " RESET "%s\n", asctime(tm));

	SSL_write(s, login, strlen(login));

	while (1 < SSL_read(s, buf, MSG_SIZE)) {
		printf(KGRN "\n Received: " RESET "%s\n", buf);
		if(strstr(buf, "35=A")) {
			printf(KGRN"\n Login OK "RESET);
		}

		if (strstr(buf, "35=0")) {
			pthread_mutex_lock(&m1);
			SSL_write(s, heart_bt, strlen(heart_bt));
			pthread_mutex_unlock(&m1);
			printf(KMAG " Heartbeat sent at " RESET "%s\n",
			       asctime(tm));
		}
	}
err:
	free(heart_bt);
	heart_bt = NULL;
	free(buf);
	buf = NULL;
	tm = NULL;
	exit(EXIT_FAILURE);
}

char *GenerateCheckSum(char *buf, long bufLen) {
	static char tmpBuf[4];
	long idx;
	unsigned int cks;

	for (idx = 0L, cks = 0; idx < bufLen; cks += (unsigned int)buf[idx++])
		;
	sprintf(tmpBuf, "%03d", (unsigned int)(cks % 256));
	return (tmpBuf);
}

void *login2(void *ctx) {
	threading *th = ctx;
	char *heart_bt = heart();

	time_t t = time(NULL);
	struct tm *tm = localtime(&t);

	char *buf = malloc(MSG_SIZE);
	if (!buf) {
		fprintf(stderr, "%s\n", "malloc-ing receiving buffer failed");
		goto err;
	}


	SSL_write(th->s, th->login, strlen(th->login));
	printf(KGRN "\n Login sent at " RESET "%s", asctime(tm));

	while (1 < SSL_read(th->s, buf, MSG_SIZE)) {
		printf(KGRN "\n Received: " RESET "%s\n", buf);
		if(strstr(buf, "35=A")) {
		printf(KGRN" Login OK\n "RESET);
		}

		if (strstr(buf, "35=0")) {
			pthread_mutex_lock(&m1);
			SSL_write(th->s, heart_bt, strlen(heart_bt));
			pthread_mutex_unlock(&m1);
			printf(KMAG " Heartbeat sent at " RESET "%s\n",
			       asctime(tm));
		}
	}
err:
	free(heart_bt);
	heart_bt = NULL;
	free(buf);
	buf = NULL;
	tm = NULL;
	exit(EXIT_FAILURE);
	return NULL;
}

int main(void) {
	pthread_mutex_init(&m1, NULL);

	SSL *s = ssl_conn(sockfd(HOST, PORT), HOST);

	message *m = start_message();

	pthread_t t1;

	threading td = {.s = s, .login = m->tmp[7]};

	pthread_create(&t1, NULL, login2, &td);
	pthread_join(t1, NULL);

	//	login(s, m->tmp[7]);

	pthread_mutex_destroy(&m1);
}

