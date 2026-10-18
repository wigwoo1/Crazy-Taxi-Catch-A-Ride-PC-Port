#include "tip_hud.h"
#include <stdio.h>
#include <string.h>
static int32_t bits(uint32_t u){int32_t s;memcpy(&s,&u,4);return s;}
static int32_t word(const uint8_t *p){return bits(p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24));}
static int sb(uint8_t v){return v<128?v:(int)v-256;}
static int32_t asr8(int32_t v){uint32_t u=(uint32_t)v;return bits((u>>8)|(u&0x80000000u?0xff000000u:0));}
int taxi_tip_hud_requests(const uint8_t tips[10][56],uint16_t frame,uint8_t paused,TaxiTipHudRequest out[10]){
 if(!tips||!out)return -1;
 const uint32_t images[3]={0x080ea67c,0x080e9474,0x080e826c};int n=0;
 for(unsigned i=0;i<10;i++){
  const uint8_t *p=tips[i];if(!sb(p[2]))continue;int kind=sb(p[0]);if(kind<0||kind>2)return -1;
  TaxiTipHudRequest q={0};q.image_x=asr8(word(p+8));q.image_y=asr8(word(p+12));if(q.image_y<17)q.image_y=17;
  q.image_address=images[kind]+(uint32_t)(sb((uint8_t)(frame>>8))*256);
  q.text_x=asr8(word(p+32));q.text_y=asr8(word(p+36));q.color=(uint8_t)(paused?1:kind==0?2:kind==1?3:1);
  int fraction=sb(p[1]);snprintf(q.text,sizeof q.text,"$%d.%d%d",word(p+4),fraction/10,fraction%10);
  out[n++]=q;
 }
 return n;
}
