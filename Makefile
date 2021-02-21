#merge: sock.c merge_unrolled.c
#	gcc-10 -std=c2x -g -O3 -I/Users/alex/tech/include -I/Users/alex/tech/root/include/openssl -L/Users/alex/tech/root/lib -lcrypto -lssl -lpthread sock.c merge_unrolled.c -o merge
#clean: 
#	rm -rf merge merge.dSYM a.out




IFLAGS="-I/usr/local/opt/curl/include"
IFLAGS+='-I/Users/alex/tech/root/include/openssl'
IFLAGS+="-I/Users/alex/tech/include" 
LFLAGS="-L/usr/local/opt/curl/lib"
LFLAGS+="-L/Users/alex/tech/root/lib"
CFLAGS= -std=c2x -g -O3 -lcrypto -lssl -lxlsxwriter -lpthread -lcurl
CC=gcc-10

FIX: sock.c merge_unrolled.c
	$(CC) $(CFLAGS) $(IFLAGS) $(LFLAGS) sock.c merge_unrolled.c -o FIX
clean:
	rm -rf FIX FIX.dSYM sock.dSYM merge_unrolled.dSYM a.out *.dSYM merge
