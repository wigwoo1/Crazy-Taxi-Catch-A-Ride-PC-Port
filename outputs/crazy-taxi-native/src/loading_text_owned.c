#include "loading_text_owned.h"
#include "text_layout.h"
#include <string.h>
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static uint16_t half(const uint8_t *p){return (uint16_t)(p[0]|(unsigned)p[1]<<8);}
static int32_t bits(uint32_t v){int32_t s;memcpy(&s,&v,4);return s;}
static const uint8_t *range(TaxiObjQueueOwner *o,uint32_t id,size_t n){if(id<0x08000000)return NULL;size_t off=id-0x08000000;if(off>o->rom_size||n>o->rom_size-off)return NULL;return o->rom+off;}
int taxi_loading_text_owned(TaxiObjQueueOwner *o,const TaxiFontSetup *fonts,const uint32_t a[9]){
 if(!o||!o->rom||!o->queue||!fonts||!a||(uint8_t)a[0]||a[4]||a[5]||a[6]!=4||a[7]||a[8]!=256)return -1;
 const uint8_t *d=fonts->descriptors[0];if(word(d+8)!=32)return -1;
 unsigned group=(uint8_t)word(d+4);const uint8_t *r=range(o,o->resource+4+group*4,4);if(!r)return -1;
 uint32_t gid=o->resource+word(r);r=range(o,gid,20);if(!r)return -1;uint32_t count=word(r);if(!count||count>256)return -1;
 uint16_t metrics[256][9];for(unsigned i=0;i<count;i++){
  r=range(o,gid+20+i*4,4);if(!r)return -1;r=range(o,o->resource+word(r),18);if(!r)return -1;
  for(unsigned j=0;j<9;j++)metrics[i][j]=half(r+j*2);
 }
 uint32_t pairid=word(d+12);const uint8_t *pairs=pairid?range(o,pairid,2+(size_t)count*count):NULL;if(pairid&&!pairs)return -1;if(pairs)pairs+=2;
 r=range(o,a[3],1);if(!r)return -1;size_t off=a[3]-0x08000000,length=0;
 while(length<o->rom_size-off&&length<4096&&r[length]){if(r[length]<32||(unsigned)r[length]-32>=count)return -1;++length;}
 if(length>=o->rom_size-off||length>=4096)return -1;
 int32_t spacing=bits(word(d+16));uint32_t measure=taxi_text_measure(&metrics[0][0],(uint16_t)count,(const char *)r,pairs,spacing);
 uint32_t x=(a[1]<<8)-(((measure>>16)*256u)>>1),y=a[2]<<8;
 for(size_t i=0;i<length;i++){
  unsigned glyph=r[i]-32,next=r[i+1]?r[i+1]-32:0;
  if(glyph){uint32_t pos[3]={(uint32_t)(bits(x)/256),(uint32_t)(bits(y)/256),0};TaxiStartupObjQueued queued;
   if(taxi_obj_queue_owned(o,glyph,group,pos,0,0,0,0,&queued)<0)return -1;
  }else o->queue->last_width=(uint8_t)(metrics[0][0]*8);
  x+=(uint32_t)o->queue->last_width*256-(uint32_t)taxi_text_correction_q8(&metrics[0][0],(uint16_t)count,(uint16_t)glyph,(uint16_t)next,pairs,spacing,256);
 }
 return 0;
}
