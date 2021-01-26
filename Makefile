merge: sock.c merge_unrolled.c
	gcc-10 -std=c2x -g -O3 -I/Users/alex/tech/include -I/Users/alex/tech/root/include/openssl -L/Users/alex/tech/root/lib -lcrypto -lssl -lpthread sock.c  merge_unrolled.c -o merge
clean: 
	rm -rf merge merge.dSYM
