#include "obj_queue_owned.h"
#include <string.h>
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static uint16_t half(const uint8_t *p){return (uint16_t)(p[0]|(unsigned)p[1]<<8);}
static int32_t bits(uint32_t v){int32_t s;memcpy(&s,&v,4);return s;}
static const uint8_t *range(TaxiObjQueueOwner *o,uint32_t id,size_t n){if(id<0x08000000)return NULL;size_t off=id-0x08000000;if(off>o->rom_size||n>o->rom_size-off)return NULL;return o->rom+off;}
int taxi_obj_queue_owned(TaxiObjQueueOwner *o,uint32_t frame,uint32_t group,const uint32_t position[3],uint32_t priority,uint32_t bucket,uint32_t flags,uint32_t palette,TaxiStartupObjQueued *out){
 if(!o||!o->rom||!o->queue||!o->group_bases||!o->palette_bases||!position||!out)return -1;
 if((int8_t)o->queue->total<0)return 0;
 const uint8_t *r=range(o,o->resource,4);if(!r)return -1;
 unsigned g=(uint8_t)group,f=(uint8_t)frame;if(g>=word(r))return -1;if(g>=o->base_count)return -1;
 r=range(o,o->resource+4+g*4,4);if(!r)return -1;uint32_t gid=o->resource+word(r);r=range(o,gid,20);if(!r)return -1;
 if(f>=word(r))return -1;uint32_t units=word(r+4);r=range(o,gid+20+f*4,4);if(!r)return -1;
 r=range(o,o->resource+word(r),18);if(!r)return -1;uint16_t m[9];for(unsigned i=0;i<9;i++)m[i]=half(r+i*2);
 uint32_t tile=(o->group_bases[g]+0xf9ff0000u)>>5;TaxiStartupObjQueue next=*o->queue;
 int result=units==16?taxi_title_obj_queue(&next,m,bits(position[0]),bits(position[1]),bits(position[2]),o->width,o->height,(uint8_t)bucket,(uint8_t)priority,(uint8_t)flags,o->object_mask,tile,o->palette_bases[g],(uint8_t)palette,out):taxi_startup_obj_queue(&next,m,bits(position[0]),bits(position[1]),bits(position[2]),o->width,o->height,(uint8_t)bucket,(uint8_t)priority,(uint8_t)flags,o->object_mask,tile,out);
 if(result>=0)*o->queue=next;return result;
}
