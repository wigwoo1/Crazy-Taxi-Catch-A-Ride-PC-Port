#ifdef NDEBUG
#undef NDEBUG
#endif
#include "effect_pool.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
static unsigned calls,emits,order[9];
static void callback(void *c,TaxiEffectPool *p,unsigned i,uint32_t tag){
 (void)c;assert(tag==1);order[calls++]=i;
 if(i==0){p->records[8].bytes[0x34]=5;p->records[8].bytes[0x3a]=2;p->records[8].bytes[0x3c]=1;}
}
static void emit(void *c,TaxiEffectPool *p,unsigned type){(void)c;assert(type==2||type==3);assert(p->slots[type-2][0]==-1);p->records[4].bytes[0x34]=7;emits++;}
static void begin(TaxiEffectPool *p){memset(p,0,sizeof *p);for(unsigned g=0;g<2;g++)for(unsigned j=0;j<6;j++)p->slots[g][j]=-1;}
int main(void){
 TaxiEffectPool p;begin(&p);
 unsigned cold_ring=8;int32_t boost_timer=999;
 assert(taxi_effect_pool_cold_init(&p,&cold_ring,&boost_timer)==0&&cold_ring==0&&boost_timer==0);
 for(unsigned g=0;g<2;g++)for(unsigned j=0;j<6;j++)assert(p.slots[g][j]==-1);
 cold_ring=8;assert(taxi_effect_pool_cold_init(&p,&cold_ring,NULL)==-1&&cold_ring==8);
 unsigned ring=8;memset(p.records,255,sizeof p.records);p.slots[0][0]=4;
 assert(taxi_effect_pool_reset(&p,&ring)==0&&ring==0&&p.slots[0][0]==4);
 for(unsigned i=0;i<9;i++)for(unsigned j=0;j<72;j++)assert(p.records[i].bytes[j]==0);
 ring=7;assert(taxi_effect_pool_reset(NULL,&ring)==-1&&ring==7);
 begin(&p);
 p.records[0].bytes[0x34]=5;p.records[0].bytes[0x3a]=2;p.records[0].bytes[0x3c]=1;
 assert(!taxi_effect_pool_step(&p,1,callback,emit,0)&&calls==2&&order[0]==0&&order[1]==8);
 assert(p.records[8].bytes[0x3a]==1);
 begin(&p);p.records[4].bytes[0x34]=2;p.records[4].bytes[0x3a]=1;p.slots[0][0]=p.slots[0][1]=4;
 assert(!taxi_effect_pool_step(&p,1,callback,emit,0)&&emits==1&&p.slots[0][1]==4&&!p.records[4].bytes[0x34]);
 begin(&p);p.records[4].bytes[0x34]=3;p.records[4].bytes[0x3a]=1;p.slots[1][0]=4;
 assert(!taxi_effect_pool_step(&p,1,callback,emit,0)&&emits==2&&!p.records[4].bytes[0x34]&&p.slots[1][0]==-1);
 for(unsigned t=0;t<65536;t++){
  begin(&p);p.records[3].bytes[0x34]=5;p.records[3].bytes[0x3a]=t;p.records[3].bytes[0x3b]=t>>8;
  assert(!taxi_effect_pool_step(&p,1,callback,emit,0));
  unsigned value=(t+65535)&65535;
  assert((p.records[3].bytes[0x34]!=0)==(value>0&&value<32768));
 }
 begin(&p);p.slots[1][5]=9;TaxiEffectPool old=p;
 assert(taxi_effect_pool_step(&p,1,callback,emit,0)==-1&&!memcmp(&old,&p,sizeof p));
 puts("PASS native effect pool: nine live records, ordered callback mutation, signed-halfword lifetime wrap, first-match unlink and expiry emission ordering; visuals pending.");
}
