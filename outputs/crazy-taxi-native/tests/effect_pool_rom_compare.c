#include "../src/effect_pool.h"
#include <stdio.h>
#include <string.h>
static int events[100],count,mode;
static void cb(void*c,TaxiEffectPool*p,unsigned i,uint32_t tag){(void)c;(void)p;(void)tag;events[count++]=(int)i;}
static void emit(void*c,TaxiEffectPool*p,unsigned type){(void)c;events[count++]=100+(int)type;if(mode){p->records[8].bytes[0x34]=1;p->records[8].bytes[0x3a]=10;p->records[8].bytes[0x3b]=0;}}
int main(void){unsigned life,pattern;int delta;while(scanf("%u%d%u%d",&life,&delta,&pattern,&mode)==4){TaxiEffectPool p;memset(&p,0x5a,sizeof p);count=0;for(unsigned i=0;i<9;i++){p.records[i].bytes[0x34]=(uint8_t)(i%5+1);p.records[i].bytes[0x3a]=(uint8_t)life;p.records[i].bytes[0x3b]=(uint8_t)(life>>8);p.records[i].bytes[0x3c]=0x11;p.records[i].bytes[0x3d]=0x11;p.records[i].bytes[0x3e]=0;p.records[i].bytes[0x3f]=0;}for(unsigned g=0;g<2;g++)for(unsigned i=0;i<6;i++)p.slots[g][i]=(pattern&(1<<i))?(int)i:-1;if(taxi_effect_pool_step(&p,delta,cb,emit,0))return 1;printf("%d",count);for(int i=0;i<count;i++)printf(" %d",events[i]);for(unsigned g=0;g<2;g++)for(unsigned i=0;i<6;i++)printf(" %d",p.slots[g][i]);for(unsigned i=0;i<648;i++)printf(" %u",((unsigned char*)p.records)[i]);puts("");}return 0;}
