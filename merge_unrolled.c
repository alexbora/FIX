#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <omp.h>
#include <sys/time.h>
#include <unistd.h>
//#include <apex_memmove.h>
#include "merge_unrolled.h"
#include <allheaders.h>

#define API (const uint8_t*)"c537779d863d054eb803522a53fac09c"
#define SECRET (const uint8_t*)"Th/ydPigXK7kdROh31J9tgBoCsU6kgOc3WDINrGBotzcRaVlWekuNFGK1sVnS3GVG2Jp1wTHASLjIPUcVclkbw=="
#define SLEN strlen((const char*)SECRET)

unsigned checksum(const unsigned char *buffer, const size_t len, unsigned seed)
{
   for (size_t i = 0; i < len; ++i)
      seed += (unsigned)(*buffer++);
   return seed % 256;
}

account* account_new(logon *login){
   account *out = malloc(sizeof(*out));
   *out = (account){.dec = malloc(BUF), .key = NULL};
   EVP_DecodeBlock(out->dec, login->secret, strlen(login->secret));
   out->key = EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, NULL, out->dec, strlen((const char*)out->dec));
   return out;
}

message *message_new2(int const size, const account *a){
   message *out = malloc(sizeof(message));
   *out = (message){.data = malloc(BUF), .tmp = malloc(sizeof(char*)*size)};
   for (int i = 0; i < size; i++)out->tmp[i] = malloc(BUF*2);
   out->mdctx = EVP_MD_CTX_create();
   EVP_DigestSignInit(out->mdctx, NULL, EVP_sha256(), NULL, a->key);
   out->t = malloc(sizeof(struct tm));
   return out;
}

void generate_msg(const account *a1, message *m, const logon *login){
   m->tmp[1] = "A";
   m->tmp[2] = "1";
   time_t rawtime = time(0);
   struct tm* const restrict t = localtime(&rawtime);
   sprintf((char*)m->tmp[0], "%d" "%02d" "%02d-" "%02d:"  "%02d:"  "%02d" ,
         t->tm_year+1900,
         t->tm_mon+1,
         t->tm_mday,
         t->tm_hour,
         t->tm_min,
         t->tm_sec
         );
   sprintf(m->tmp[3], "%s\1%s\1%s\1%s\1Coinbase\1%s",
         m->tmp[0],
         m->tmp[1],
         m->tmp[2],
         login->api,
         login->pass);
   EVP_DigestSignUpdate(m->mdctx, m->tmp[3], strlen(m->tmp[3]));
   EVP_DigestSignFinal(m->mdctx, NULL, &m->len);
   EVP_DigestSignFinal(m->mdctx, m->tmp[4], &m->len);
   EVP_EncodeBlock(m->tmp[5], m->tmp[4], m->len);

   sprintf(m->tmp[6],"49=%s|554=%s|96=%s|8013=S|52=%s|56=Coinbase|98=0|108=30|34=1|9406=N|",
         login->api,
         login->pass,
         m->tmp[5],
         m->tmp[0]
         );
   size_t len_h1 = strlen("35=|") + strlen(m->tmp[1]) + strlen(m->tmp[6]);
   sprintf(m->tmp[7], "8=FIX.4.2|9=%d|35=%s|", len_h1, m->tmp[1]);
   strcat(m->tmp[7], m->tmp[6]);
   for(size_t i=0;i<strlen(m->tmp[7]);i++)(m->tmp[7][i] == BAR)?m->tmp[7][i]=SOH :0;
   m->checksum = checksum(m->tmp[7], strlen(m->tmp[7]), 0);
   sprintf(m->tmp[8], "10=%d\1", m->checksum);
   strcat((char*)m->tmp[7], (char*)m->tmp[8]);

}

message *start_message(){
   logon login = {"c537779d863d054eb803522a53fac09c", "Th/ydPigXK7kdROh31J9tgBoCsU6kgOc3WDINrGBotzcRaVlWekuNFGK1sVnS3GVG2Jp1wTHASLjIPUcVclkbw==", "012345"};
   logon login2 = {"dc9ebb680625f99c2b17c572fb86e6b3","8hgmPq1g718dLrihpKk1x0+IKQiyQkwijVdYH8iOWlfA78iCbMuDk2yE3akcFildqjnltjrK5SyvHLRWCC4w8g==", "012345"};
   account *a1 = account_new(&login); free(a1);
   account *a2 = account_new(&login2);
   message *m = message_new2(9, a2);
   generate_msg(a2, m, &login2);
   return m;
}


#ifdef test
int main(){

   account *a1 = account_new(&login);
   message *m = message_new2(9, a1);
   generate_msg(a1, m, &login);
   /*
      time_t t = 0; time(&t);
      COLOR_PRINT(ANSI_GREEN, ctime(&t));

      account *a1 = account_new(&login);
      message *m = message_new2(9, a1);

      clock_t start = 0;
      start = clock();
      for (int i = 0; i<1e6; i++){generate_msg(a1, m, &login);}
      printf("time: %f\n", (float)(clock()-start)/CLOCKS_PER_SEC);

      time(&t);
      COLOR_PRINT(ANSI_MAGENTA, "----finished----");
      COLOR_PRINT(ANSI_MAGENTA, ctime(&t));
      free(a1); free(m);
      char *tei = NULL;
      free(tei);
      */
}
#endif
