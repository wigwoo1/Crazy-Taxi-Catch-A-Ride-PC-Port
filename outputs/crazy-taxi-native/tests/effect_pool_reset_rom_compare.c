#include "effect_pool.h"
#include <assert.h>
#include <stdio.h>
int main(void){unsigned seed;while(scanf("%u",&seed)==1){TaxiEffectPool pool;unsigned ring=seed;
 assert(sizeof pool==696);unsigned char *bytes=(unsigned char *)&pool;for(unsigned i=0;i<sizeof pool;i++)bytes[i]=(unsigned char)(seed+i*37);
 assert(!taxi_effect_pool_reset(&pool,&ring));for(unsigned i=0;i<sizeof pool;i++)printf("%02x",bytes[i]);printf(" %u\n",ring);
}return 0;}
