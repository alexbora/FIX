#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/evp.h>
#include <fcntl.h>
#include <openssl/ssl.h>
#include <pthread.h>


#include <allheaders.h>
#include "merge_unrolled.h"

#define _GNU_SOURCE
#ifndef BUFISZE
#define BUFSIZE LWS_PRE + 64
#endif

#include <sys/socket.h>
#include <netdb.h>

#include <sys/signal.h>
#ifdef WIN32
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#define close closesocket
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#define h_addr h_addr_list[0] /* for backward compatibility */


#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif

volatile int FLAG = 0;

static inline
int sockfd(const char hostname[], const int port){
	register int sockfd = 0;
	struct  sockaddr_in servAddr = {0};

#ifdef WIN32
	WSADATA wsaData;
	int initwsa = WSAStartup(MAKEWORD(2, 0), &wsaData);
	if(initwsa != 0) {
		printf("WSAStartup failed: %d\n", initwsa);
		return 1;
	}
#endif

	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port); //443

	struct hostent *host = NULL;
	host = gethostbyname(hostname);
	servAddr.sin_addr.s_addr = *(long*)(host->h_addr);
	memcpy(&servAddr.sin_addr.s_addr, host->h_addr, host->h_length);

	setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY,(int[]){1},  sizeof(int));
	setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (int[]){1},  sizeof(int));
	setsockopt(sockfd, SOL_SOCKET, SO_SNDLOWAT,(int[]){2}, sizeof(int));

	connect(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr));

#ifdef WIN32
	WSACleanup();
#endif

	return sockfd;
}

static inline
SSL *ssl_conn(const int sockfd, const char *hostname)
{
	SSL *ssl = NULL;
	SSL_library_init();
	const SSL_METHOD* method = TLS_client_method();
	SSL_CTX* ctx = SSL_CTX_new(method);
	ssl = SSL_new(ctx);
	SSL_set_fd(ssl, sockfd);
	SSL_set_tlsext_host_name(ssl, hostname);
	SSL_connect(ssl);
	return ssl;
}

void format_time(char *buf)
{
	time_t rawtime = time(0);
	struct tm* const restrict t = localtime(&rawtime);
	sprintf((char*)buf, "%d" "%02d" "%02d-" "%02d:"  "%02d:"  "%02d" ,
			t->tm_year+1900,
			t->tm_mon+1,
			t->tm_mday,
			t->tm_hour,
			t->tm_min,
			t->tm_sec
	       );
}


char *heart(){
	char *heartbeat = malloc(256);
	char timestamp[64], tmp[64];

	format_time(timestamp);

	size_t length = strlen("35=0\1") + strlen("TEST\1") + strlen("49=Coinbase^56=Coinbase^34=4\1") + strlen(timestamp);
	
	sprintf(heartbeat, "8=FIX.4.2^9=%ld^35=0^49=Coinbase^56=Coinbase^34=4^52=%s^112=TEST^", length, timestamp);
	
	int chk = checksum(heartbeat, strlen(heartbeat), 0);
	
	sprintf(tmp, "10=%d^", chk);
	strcat(heartbeat, tmp);
	
	for (size_t i=0; i<strlen(heartbeat); i++)(heartbeat[i] == '^') ? heartbeat[i] = '\1': 0;
	
	return heartbeat;
}


void login(SSL *s, char *login){
	char *heart_bt = heart(); char *buf = malloc(MSG_SIZE);
	
	SSL_write(s, login, strlen(login));
	
	while (1<SSL_read(s, buf, MSG_SIZE)){
		printf(KGRN"Received: "RESET"%s\n", buf);
		while(strstr(buf, "35=1")){
			SSL_write(s, heart_bt, strlen(heart_bt));
		}
	}
	free(heart_bt); free(buf);
}


int main(void){

	char *hostname = "fix.gdax.com";
	SSL *s = ssl_conn(sockfd(hostname, 4198), hostname);
	
	char *buf = malloc(MSG_SIZE);
	message *m = start_message();
	login(s, m->tmp[7]);
}

