#include "sprite_replacement_owned.h"
#include <string.h>
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static uint16_t half(const uint8_t *p){return (uint16_t)(p[0]|(unsigned)p[1]<<8);}
static const uint8_t *range(const TaxiSpriteReplacementOwner *o,uint32_t id,size_t bytes){
 if(id<0x08000000)return NULL;size_t off=id-0x08000000;if(off>o->rom_size||bytes>o->rom_size-off)return NULL;return o->rom+off;
}
int taxi_sprite_replace_owned(TaxiSpriteReplacementOwner *o,uint32_t source,uint32_t index,uint32_t frame,uint32_t group,uint32_t immediate){
 if(!o||!o->rom||!o->copies||!o->immediate||!o->group_bases)return -1;
 const uint8_t *resource=range(o,o->resource,4);if(!resource)return -1;
 TaxiSpriteCopyAsset a={0};a.group_count=word(resource);
 if((uint16_t)group<a.group_count){
  const uint8_t *slot=range(o,o->resource+4+4u*(uint16_t)group,4);if(!slot||(uint16_t)group>=o->base_count)return -1;
  uint32_t gid=o->resource+word(slot);const uint8_t *g=range(o,gid,20);if(!g)return -1;
  a.frame_count=word(g);a.palette_units=word(g+4);a.destination_base=o->group_bases[(uint16_t)group];
  if((uint16_t)frame<a.frame_count){
   slot=range(o,gid+20+4u*(uint16_t)frame,4);if(!slot)return -1;
   const uint8_t *m=range(o,o->resource+word(slot),18);if(!m)return -1;
   a.width_tiles=half(m);a.height_tiles=half(m+2);a.tile_index=half(m+16);
  }
 }
 TaxiSpriteCopies next=*o->copies;TaxiSpriteImmediateCopy dma=*o->immediate;
 int result=taxi_sprite_replace(&next,&a,source,index,frame,group,immediate,&dma);if(result<0)return -1;
 if(result&&(uint8_t)immediate){
  size_t bytes=(size_t)(dma.control&0xffff)*4;const uint8_t *from=range(o,dma.source,bytes);
  if(!from||!o->objects||dma.destination<o->object_identity)return -1;
  size_t off=dma.destination-o->object_identity;if(off>o->object_size||bytes>o->object_size-off)return -1;
  memcpy(o->objects+off,from,bytes);
 }
 *o->copies=next;*o->immediate=dma;return result;
}
