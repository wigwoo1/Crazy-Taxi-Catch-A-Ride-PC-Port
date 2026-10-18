#include "restart_block_reset.h"
#include <assert.h>
#include <stdio.h>
int main(void){unsigned seed;while(scanf("%u",&seed)==1){TaxiRestartBlockReset s;
 s.word_1768=(int32_t)seed;s.word_176c=seed^0xabcdef01;for(unsigned i=0;i<512;i++)s.block_2b8ec[i]=(uint8_t)(seed+i*37);
 assert(!taxi_restart_block_reset(&s));printf("%d %u ",s.word_1768,s.word_176c);for(unsigned i=0;i<512;i++)printf("%02x",s.block_2b8ec[i]);puts("");
}return 0;}
