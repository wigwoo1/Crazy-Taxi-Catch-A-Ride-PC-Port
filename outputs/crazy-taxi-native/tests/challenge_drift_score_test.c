#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "score_insert.h"
#include "challenge_completion.h"
static uint32_t word(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static void put(uint8_t *p,uint32_t w){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(w>>(8*i));}
int main(void){
 for(int count=-32768;count<=32767;count++)for(unsigned profile=0;profile<2;profile++){
  uint8_t rows[12],before[12];put(rows,(20u<<15)|0x7f1f);put(rows+4,(18u<<15)|3);put(rows+8,(15u<<15)|7);memcpy(before,rows,12);
  unsigned expected=count>20?1:count>18?2:count>15?3:0;
  assert(taxi_challenge_drift_insert(rows,(uint8_t)profile,7,(int16_t)count)==expected);
  if(!expected)assert(!memcmp(rows,before,12));else{
   unsigned i=expected-1;uint32_t prior=word(before+i*4);
   assert(word(rows+i*4)==((prior&30u)|1u|96u|(profile<<7)|((uint32_t)count<<15)));
   for(unsigned j=0;j<i;j++)assert(!memcmp(rows+j*4,before+j*4,4));
   for(unsigned j=i+1;j<3;j++)assert(!memcmp(rows+j*4,before+(j-1)*4,4));
  }
 }
 for(unsigned profile=0;profile<2;profile++){
  uint8_t settings[512],before[512],snapshot[512];memset(settings,0xa5,512);memset(settings+144+208*profile,0,12);memcpy(before,settings,512);
  TaxiChallengeDriftEffects effects={0};uint8_t rank=77;
  assert(!taxi_challenge_drift_complete(settings,(uint8_t)profile,2,15,&effects,&rank)&&rank==77&&!memcmp(settings,before,512));
  effects.request_rank=1;assert(taxi_challenge_drift_complete(settings,(uint8_t)profile,2,15,&effects,&rank)==1&&rank==1);
  for(unsigned i=0;i<512;i++)if(i<144+208*profile||i>=156+208*profile)assert(settings[i]==before[i]);
  memcpy(snapshot,before,512);memset(settings,0x55,512);assert(taxi_challenge_drift_recalculate(settings,snapshot,(uint8_t)profile,2,15)==1);
  assert(word(settings+144+208*profile)==((15u<<15)|65u|(profile<<7)));
  for(unsigned i=0;i<512;i++)if(i<144+208*profile||i>=156+208*profile)assert(settings[i]==snapshot[i]);
  memcpy(before,settings,512);assert(taxi_challenge_drift_complete(settings,2,0,20,&effects,&rank)==-1&&!memcmp(settings,before,512));
 }
 uint8_t empty[12]={0};assert(taxi_challenge_drift_insert(empty,0,0,-1)==1&&word(empty)==0xffff8001u);
 puts("PASS original Drift scoring:131072 signed count/profile rankings, ties/shift/metadata/clearfraction, own group144/352, gated request and snapshot restoration; controlled fixtures, not earned driving.");return 0;
}
