#include "credits_flow.h"
#include <string.h>
static int32_t bits(uint32_t v){int32_t s;memcpy(&s,&v,4);return s;}
static int32_t asr(int32_t v,unsigned n){return bits(((uint32_t)v>>n)|(v<0?~0u<<(32-n):0));}
void taxi_credits_flow_begin(TaxiCreditsFlow *s){if(s)*s=(TaxiCreditsFlow){.y_q8=176*256,.row=704,.timer=0};}
int taxi_credits_flow_step(TaxiCreditsFlow *out,int32_t delta,int32_t sine,
 const uint8_t text[31],uint8_t effects,int confirm,int start,TaxiCreditsFrame *frame){
 if(!out||!frame)return -1;
 TaxiCreditsFlow s=*out;TaxiCreditsFrame f={.text_id=-1};
 uint32_t step=(uint32_t)delta<<7;
 s.y_q8=bits((uint32_t)s.y_q8-step);s.x_q8=bits((uint32_t)s.x_q8+step);
 s.line_q8=bits((uint32_t)s.line_q8+step);
 s.timer=(uint8_t)(s.timer-1);
 if(s.line_q8>2047){
  if(s.line<=103&&!text)return -1;
  s.line_q8=0;f.write_row=1;
  if(s.line<=103){
   f.text_id=(int32_t)(750+s.line);
   uint16_t palette=(uint16_t)(0xffd4u+text[0]);
   for(unsigned i=0;i<30;i++)f.tiles[i]=(uint16_t)(((uint32_t)palette<<12)|(uint32_t)((int)text[i+1]-32));
  }
  s.row=(s.row+32)&1023;s.line++;
  if(s.line==103&&!s.sound_started){s.sound_started=1;f.sound29=effects!=0;}
 }
 /* Source issues both calls if automatic exit and Confirm coincide. */
 f.fade_out=(uint8_t)((s.y_q8<=-212992)+(confirm||start));
 f.scroll[0]=(uint16_t)((uint32_t)asr(s.x_q8,8)&255);
 f.scroll[1]=(uint16_t)asr(sine,3);f.scroll[2]=s.timer;
 s.angle=(s.angle+32)&4095;
 *out=s;*frame=f;return 0;
}
