#include "challenge_flag_hud.h"
#include <string.h>
int taxi_challenge_flag_hud_plan(TaxiFlagHudPlan *out,int16_t remaining,uint16_t height,uint8_t flags){
 if(!out)return -1;
 TaxiFlagHudPlan p={0};int32_t ticks=remaining;
 int minutes=0,seconds=0,hundredths=0;
 if(ticks>0){minutes=ticks/3600;ticks-=minutes*3600;seconds=ticks/60;ticks-=seconds*60;hundredths=ticks*100/60;
  if(minutes>59){minutes=seconds=59;hundredths=99;}}
 p.time[0]=(char)('0'+minutes/10);p.time[1]=(char)('0'+minutes%10);p.time[2]=39;
 p.time[3]=(char)('0'+seconds/10);p.time[4]=(char)('0'+seconds%10);p.time[5]='"';
 p.time[6]=(char)('0'+hundredths/10);p.time[7]=(char)('0'+hundredths%10);
 p.indicator_y=(int32_t)height-23;p.indicator_variant=(flags&32)?2:1;
 for(unsigned i=0;i<5;i++){p.glyphs[i].glyph=(uint8_t)((uint8_t)p.time[7-i]-32);p.glyphs[i].scaled=(uint8_t)(i<2);p.glyphs[i].position_identity=0x087ec15c-i*12;}
 *out=p;return 0;
}


int taxi_challenge_flag_hud_positions(TaxiFlagHudPlan *plan,const uint8_t *records,size_t size){
 if(!plan||!records||size!=60)return -1;
 TaxiFlagHudPlan next=*plan;
 for(unsigned i=0;i<5;i++){
  int32_t values[3];
  for(unsigned j=0;j<3;j++){const uint8_t *p=records+(4-i)*12+j*4;uint32_t raw=p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);memcpy(values+j,&raw,4);}
  next.glyphs[i].x=values[0];next.glyphs[i].y=values[1];next.glyphs[i].z=values[2];
 }
 *plan=next;return 0;
}
