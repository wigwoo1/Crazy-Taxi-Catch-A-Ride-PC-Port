#include "effect_pool.h"
#include <assert.h>
#include <stdio.h>
int main(void){for(unsigned seed=0;seed<16;seed++){
 TaxiEffectPool p;unsigned char *bytes=(unsigned char *)&p;for(unsigned i=0;i<sizeof p;i++)bytes[i]=(unsigned char)(seed+i*37);
 assert(!taxi_effect_pool_session_slots_reset(&p));for(unsigned i=0;i<648;i++)printf("%02x",bytes[i]);
 for(unsigned g=0;g<2;g++)for(unsigned j=0;j<6;j++)printf(" %d",p.slots[g][j]);puts("");
}return 0;}
