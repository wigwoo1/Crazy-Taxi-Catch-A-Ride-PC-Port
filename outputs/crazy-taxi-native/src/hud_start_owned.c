#include "hud_start_owned.h"
#include <string.h>
typedef struct {TaxiHudStartOwners *owner;int error;} Context;
static uint32_t resource(void *opaque,uint32_t id){
 Context *c=opaque;TaxiOrdinaryGraphicsOwners *g=c->owner->graphics;
 if(id<0x08000000){c->error=1;return 0;}
 size_t off=id-0x08000000;if(off>g->rom_size||g->rom_size-off<4){c->error=1;return 0;}
 const uint8_t *p=g->rom+off;return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
}
static void write_owner(void *opaque,uint32_t id,uint32_t value,unsigned size){
 Context *c=opaque;TaxiHudStartOwners *o=c->owner;uint8_t *p=NULL;
 if(id>=0x020016c0&&id+size<=0x020016e0)p=o->scalar_16c0+id-0x020016c0;
 else if(id>=0x02005300&&id+size<=0x02005400)p=o->cache_5300+id-0x02005300;
 else if(id>=0x03005490&&id+size<=0x03005498)p=o->sprite_5490+id-0x03005490;
 else if(id>=0x02001790&&id+size<=0x02001790+128*20)p=(uint8_t *)o->graphics->palette->records+id-0x02001790;
 if(!p||size>4){c->error=1;return;}
 for(unsigned j=0;j<size;j++)p[j]=(uint8_t)(value>>(j*8));
}
static int replace(void *opaque,uint32_t src,unsigned index,unsigned frame,unsigned group,unsigned immediate){
 Context *c=opaque;TaxiHudStartOwners *o=c->owner;TaxiOrdinaryGraphicsOwners *g=o->graphics;
 TaxiSpriteReplacementOwner r={g->rom,g->rom_size,0x08070e50,g->assets->bases,11,o->copies,o->immediate,g->assets->objects,sizeof g->assets->objects,g->destination};
 if(c->error)return -1;
 return taxi_sprite_replace_owned(&r,src,index,frame,group,immediate)<0?-1:0;
}
int taxi_hud_start_owned(TaxiHudStartOwners *o,const TaxiHudStartInput *i){
 if(!o||!i)return -1;
 if(i->demo)return 0;
 if(!o->graphics||!o->graphics->rom||!o->graphics->assets||!o->graphics->palette||!o->copies||!o->immediate)return -1;
 if(o->graphics->rom_size<0x7ec25c||o->graphics->palette->count<13)return -1;
 Context c={o,0};TaxiHudStartBackend b={&c,resource,write_owner,replace};
 int rc=taxi_hud_start(i,&b);return rc||c.error?-1:0;
}
