#merge: sock.c merge_unrolled.c
#	gcc-10 -std=c2x -g -O3 -I/Users/alex/tech/include -I/Users/alex/tech/root/include/openssl -L/Users/alex/tech/root/lib -lcrypto -lssl -lpthread sock.c merge_unrolled.c -o merge
#clean: 
#	rm -rf merge merge.dSYM a.out




IFLAGS="-I/usr/local/opt/curl/include"
IFLAGS+='-I/Users/alex/tech/root/include/openssl'
IFLAGS+="-I/Users/alex/tech/include" 
LFLAGS="-L/usr/local/opt/curl/lib"
LFLAGS+="-L/Users/alex/tech/root/lib"

CFLAGS= -std=c2x -g -O3 -lcrypto -lssl -lxlsxwriter -lpthread -lcurl -fstack-protector-strong -D_FORTIFY_SOURCE=1 #-D_POSIX_C_SOURCE=200809L
CC=gcc-10

FIX: sock.c merge_unrolled.c utils.c
	$(CC) $(IFLAGS) $(LFLAGS) sock.c merge_unrolled.c utils.c test_x3.c -o FIX2  $(CFLAGS) 

.c:
		$(CC) $(IFLAGS) $(LFLAGS) -o $@ $< $(CFLAGS)
clean:
	rm -rf FIX a.out *.dSYM merge
include config.mk
