#include "vehicle_start_finalize.h"
#include <stdio.h>
#include <assert.h>
int main(void){unsigned seed;int base;while(scanf("%u %d",&seed,&base)==2){uint8_t a[0xea0];for(unsigned i=0;i<sizeof a;i++)a[i]=(uint8_t)(seed+i*37);a[0x532]=(uint8_t)base;a[0x533]=(uint8_t)((unsigned)base>>8);uint32_t rng=seed;
 assert(!taxi_vehicle_start_finalize(a,sizeof a,&rng));for(unsigned i=0;i<sizeof a;i++)printf("%02x",a[i]);printf(" %u\n",rng);
 }return 0;}
