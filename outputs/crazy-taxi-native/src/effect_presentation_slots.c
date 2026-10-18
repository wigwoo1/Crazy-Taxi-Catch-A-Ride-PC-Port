#include "effect_presentation_slots.h"
#include <string.h>
int taxi_effect_presentation_slots(const TaxiEffectRecord records[9],TaxiPoint camera,int16_t pitch,int16_t yaw,int16_t roll,const TaxiTrig *trig,int32_t phase,const TaxiEffectSprite *requests,unsigned count,TaxiEffectPresentationSlot out[9]){
 if(!records||!trig||!out||count>9||(count&&!requests))return -1;
 TaxiEffectPresentationSlot slots[9]={0};unsigned used=0;
 for(unsigned n=0;n<9;n++){
  slots[n].record=records[n];TaxiEffectSprite probe;int rc=taxi_effect_world_sprite(records+n,camera,pitch,yaw,roll,trig,192,120,80,phase,1,0,0,&probe);
  if(rc<0)return -1;if(!rc)continue;if(used>=count)return -1;
  const TaxiEffectSprite *r=requests+used;
  if(r->group!=probe.group||r->palette!=probe.palette||r->flags!=probe.flags||r->bucket!=probe.bucket)return -1;
  slots[n].visible=1;slots[n].sprite=*r;slots[n].unjittered=probe;used++;
 }
 if(used!=count)return -1;memcpy(out,slots,sizeof(slots));return (int)used;
}
