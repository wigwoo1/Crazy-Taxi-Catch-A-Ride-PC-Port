#include "../src/effect_presentation_slots.h"
#include "../src/random.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static uint32_t rng;static unsigned calls;
static int32_t random_value(void *unused,int32_t lo,int32_t hi){(void)unused;calls++;return taxi_random_range(&rng,lo,hi);}
static void put(uint8_t *p,int32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)((uint32_t)v>>(8*i));}
static void reject(const TaxiEffectRecord records[9],const TaxiTrig *trig,const TaxiEffectSprite *requests,unsigned count){
 TaxiEffectPresentationSlot out[9],before[9];memset(out,0xa5,sizeof out);memcpy(before,out,sizeof out);
 assert(taxi_effect_presentation_slots(records,(TaxiPoint){0,0,0},0,0,0,trig,113800,requests,count,out)==-1);
 assert(!memcmp(out,before,sizeof out));
}
int main(void){
 uint8_t data[16388];FILE *f=fopen("outputs/crazy-taxi-native/assets/driving.ctr","rb");assert(f&&fread(data,1,sizeof data,f)==sizeof data);fclose(f);TaxiTrig trig;taxi_trig_decode(&trig,data+4,data+8196);
 unsigned seeds[]={0,1,0x40000000u,0xffffffffu};unsigned cases=0,smoke=0,rejections=0;
 for(unsigned seed=0;seed<4;seed++)for(unsigned disabled=0;disabled<2;disabled++)for(unsigned layout=0;layout<9;layout++){
  TaxiEffectRecord records[9]={0},before[9];TaxiEffectSprite requests[9]={0};unsigned count=0,visible_smoke=0;rng=seeds[seed];calls=0;
  for(unsigned n=0;n<9;n++){
   put(records[n].bytes+4,(int32_t)n*31-120);put(records[n].bytes+8,20);put(records[n].bytes+12,n==layout?192:900+(int32_t)n*100);
   records[n].bytes[52]=(uint8_t)(n%5+1);records[n].bytes[53]=(uint8_t)(n+3);
   int rc=taxi_effect_world_sprite(records+n,(TaxiPoint){0,0,0},0,0,0,&trig,192,120,80,113800,disabled,random_value,0,requests+count);assert(rc>=0);
   if(rc){count++;visible_smoke+=records[n].bytes[52]==2||records[n].bytes[52]==3;}
  }
  assert(calls==(disabled?0:visible_smoke*3));uint32_t preserved_rng=rng;unsigned preserved_calls=calls;memcpy(before,records,sizeof records);
  TaxiEffectPresentationSlot slots[9];assert(taxi_effect_presentation_slots(records,(TaxiPoint){0,0,0},0,0,0,&trig,113800,requests,count,slots)==(int)count);
  unsigned index=0;for(unsigned n=0;n<9;n++){assert(!memcmp(&slots[n].record,records+n,sizeof records[n]));if(slots[n].visible){assert(!memcmp(&slots[n].sprite,requests+index,sizeof requests[0]));index++;}}
  assert(index==count&&!memcmp(records,before,sizeof records)&&rng==preserved_rng&&calls==preserved_calls);cases++;smoke+=visible_smoke;
  reject(records,&trig,requests,count-1);rejections++;
  TaxiEffectSprite bad[9];memcpy(bad,requests,sizeof bad);bad[0].palette^=1;reject(records,&trig,bad,count);rejections++;
  memcpy(bad,requests,sizeof bad);bad[0].bucket^=1;reject(records,&trig,bad,count);rejections++;
  memcpy(bad,requests,sizeof bad);bad[0].flags^=1;reject(records,&trig,bad,count);rejections++;
  reject(records,&trig,0,count);reject(records,&trig,requests,10);rejections+=2;
 }
 printf("PASS cached_effect_cases=%u smoke_requests=%u transactional_rejections=%u\n",cases,smoke,rejections);return 0;
}
