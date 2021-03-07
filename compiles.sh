#!/usr/bin/env sh

######################################################################
# @author      : alex (alex@mac.local)
# @file        : compiles
# @created     : Thursday Mar 04, 2021 21:52:27 EET
#
# @description : 
######################################################################

compiles ()
{
	stage="$(mktemp -d)"
	echo "$2" > "$stage/test.c"
	(cc -Werror "$1" -o "$stage/test" "$stage/test.c" >/dev/null 2>&1)
	cc_success=$?
	rm -rf "$stage"
	return $cc_success
}


if compiles "" "
	#include <stdint.h>
	#include <stdlib.h>
	int main(void)
	{
		void (*p)(void *, size_t) = arc4random_buf;
		return (intptr_t)p;
	}"
then
	echo "CFLAGS += -DHAVE_ARC4RANDOM" >> config.mk
fi

if compiles "-D_POSIX_C_SOURCE=200112L" "
	#include <stdint.h>
	#include <sys/types.h>
	#include <sys/random.h>
	int main(void)
	{
		ssize_t (*p)(void *, size_t, unsigned int) = getrandom;
		return (intptr_t)p;
	}"
then
	echo "CFLAGS += -DHAVE_GETRANDOM" >> config.mk
fi

if compiles "" "
  #include <openssl/ssl.h>
  #include <openssl/evp.h>
  int main(){
  SSL *ssl = NULL;
  //SSL_library_init();
  //const SSL_METHOD *method = TLS_client_method();
  //SSL_CTX *ctx = SSL_CTX_new(method);
	//ssl = SSL_new(ctx);
	//return ssl;
  return 0;
}"

then 
  echo "CFLAGS += -DHAVE_OPENSSL" >> config.mk
fi


if test -c /dev/random; then
	echo "CFLAGS += -DHAVE_DEVRANDOM" >> config.mk
fi

