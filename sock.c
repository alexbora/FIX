#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/evp.h>
#include <allheaders.h>
#include <fcntl.h>
#include <openssl/ssl.h>
#include "merge_unrolled.h"
#include <pthread.h>

#define _GNU_SOURCE
#ifndef BUFISZE
#define BUFSIZE LWS_PRE + 64
#endif

#include <sys/socket.h>
#include <netdb.h>
/* #include<netinet/in.h> */
/* #include<netinet/tcp.h> */
/* #include<sys/types.h> */

#include <sys/signal.h>
#ifdef WIN32
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#define close closesocket
#else
/* #include <netdb.h> */
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
	//setsockopt(sockfd, IPPROTO_TCP, TCP_QUICKACK,(int[]){1},  sizeof(int));
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


void reverse(char s[])
 {
     int i, j;
     char c;

     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
}  
void itoa(int n, char s[])
 {
     int i, sign;

     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
}  
char *heartbeat(){
	char *heartbeat = malloc(4096);
	char *begin = "8=FIX.4.2\1"; 
	char *tag  = "35=0\1";
	char len[64] = "9=";
	char checks[64] = "10=";
	char *test = "112=TestReqID\1";

	char len1[64], chk[64];
	size_t l = strlen(tag) + strlen(test);
	itoa(l, len1);
	strcat(heartbeat, begin);
	strcat(len, len1);
	strcat(heartbeat, len);
	strcat(heartbeat, tag);
	strcat(heartbeat, test);
int csum = checksum(heartbeat, strlen(heartbeat), 0);
itoa(csum, chk);
strcat(checks, chk);
strcat(checks, "\1");
strcat(heartbeat, checks);
	printf("tag: %s\n", heartbeat);
	return heartbeat;
}

typedef struct{
	SSL *s;
	char *heart;
}args;

void *parallel(void *p){
	args *pi = p;
	while (FLAG == 1){
		SSL_write(pi->s, pi->heart, strlen(pi->heart));
		FLAG = 0;
	}
return NULL;
}

int main(void)
{
	char *heart = heartbeat();
	
	char *hostname = "fix.gdax.com";
	int port = 4198;
	int sock = sockfd(hostname, port);

	int ofcmode=fcntl(sock,F_GETFL,0);
	ofcmode|=O_NDELAY;
	//   fcntl(sock,F_SETFL,ofcmode);

	SSL *s = ssl_conn(sock, hostname);
	char *buf = malloc(MSG_SIZE);
	message *m = start_message();

	args args = {s, heart};
	
pthread_t t;
pthread_create(&t, NULL, parallel, &args);
SSL_write(s, m->tmp[7], strlen(m->tmp[7]));
while(strstr(buf, "TEST10"))FLAG = 1;

	SSL_write(s, m->tmp[7], strlen(m->tmp[7]));
	while(1 < SSL_read(s, buf, MSG_SIZE)){
		printf("Response:\n%s\n", buf);
		while(strstr(buf, "TEST10"))FLAG = 1;
	}
}

#if 0
pthread_t t1;
pthread_create(&t1, NULL, myThreadFun, NULL); 
pthread_join(t1, NULL);


while (strstr(buf, "TEST10")){


	char heartbeat[64],  heartbeat2[64];
	size_t len_h1 = strlen("35=|") +  strlen("0");// + strlen(m->tmp[6]);
	sprintf(heartbeat, "8=FIX.4.2|9=%ld|35=0|", len_h1);
	for(size_t i=0;i<strlen(heartbeat);i++)(heartbeat[i] == BAR)?heartbeat[i]=SOH :0;
	int check = checksum(heartbeat, strlen(heartbeat), 0);
	sprintf(heartbeat2, "10=%d\1", check);
	strcat((char*)heartbeat, (char*)heartbeat2);
	SSL_write(s, heartbeat, strlen(heartbeat));
	//printf("heartbeat %s\n", heartbeat);
	printf("BUF: %s\n", buf);}

	}
#endif
