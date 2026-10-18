#include "effect_pool.h"
#include <string.h>
int taxi_effect_pool_session_slots_reset(TaxiEffectPool *p){
 if(!p)return -1;
 for(unsigned g=0;g<2;g++)for(unsigned j=0;j<6;j++)p->slots[g][j]=-1;
 return 0;
}
int taxi_effect_pool_cold_init(TaxiEffectPool *p,unsigned *ring,int32_t *timer){
 if(!p||!ring||!timer)return -1;
 memset(p->records,0,sizeof p->records);*ring=0;*timer=0;
 for(unsigned g=0;g<2;g++)for(unsigned j=0;j<6;j++)p->slots[g][j]=-1;
 return 0;
}
int taxi_effect_pool_reset(TaxiEffectPool *p,unsigned *ring){
 if(!p||!ring)return -1;
 memset(p->records,0,sizeof p->records);*ring=0;return 0;
}
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
int taxi_effect_pool_step(TaxiEffectPool *p,int32_t delta,
 TaxiEffectCallback callback,TaxiEffectEmit emit,void *context){
 if(!p||!callback||!emit)return -1;
 for(unsigned g=0;g<2;g++)for(unsigned j=0;j<6;j++)if(p->slots[g][j]<-1||p->slots[g][j]>8)return -1;
 for(unsigned i=0;i<9;i++){
  uint8_t *b=p->records[i].bytes;unsigned type=b[0x34];if(!type)continue;
  uint16_t timer=(uint16_t)((uint32_t)(b[0x3a]|(unsigned)b[0x3b]<<8)-(uint32_t)delta);
  b[0x3a]=(uint8_t)timer;b[0x3b]=(uint8_t)(timer>>8);
  if(timer&&timer<32768){
   uint32_t tag=word(b+0x3c);if(tag)callback(context,p,i,tag);
  }else{
   if(type==2||type==3){
    int *slots=p->slots[type-2];
    for(unsigned j=0;j<6;j++)if(slots[j]==(int)i){slots[j]=-1;break;}
    for(unsigned j=0;j<6;j++)if(slots[j]==-1){emit(context,p,type);break;}
   }
   b[0x34]=0; /* Original clears AFTER re-emission, including same-slot reuse. */
  }
 }
 return 0;
}
