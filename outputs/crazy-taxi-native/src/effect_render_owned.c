#include "effect_render_owned.h"
#include "taxi_affine_queue.h"
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static const uint8_t *range(const TaxiOrdinaryGraphicsOwners *g,size_t offset,size_t count){if(offset>g->rom_size||count>g->rom_size-offset)return 0;return g->rom+offset;}
int taxi_effect_render_owned(TaxiOrdinaryGraphicsOwners *g,const TaxiEffectSprite *r,uint16_t width,uint16_t height,uint16_t mask,TaxiStartupObjQueued *out){
 if(!g||!r||!out||!g->rom||!g->assets||!g->queue||!g->transport||r->group<8||r->group>10)return -1;
 size_t resource=0x70e50;const uint8_t *p=range(g,resource+4+r->group*4,4);if(!p)return -1;size_t group=resource+word(p);p=range(g,group,20);if(!p||word(p+4)!=16||r->frame>=word(p))return -1;
 p=range(g,group+20+r->frame*4,4);if(!p)return -1;p=range(g,resource+word(p),18);if(!p)return -1;uint16_t m[9];for(unsigned i=0;i<9;i++)m[i]=(uint16_t)(p[i*2]|(uint16_t)p[i*2+1]<<8);
 uint32_t tile=(g->assets->bases[r->group]+0xf9ff0000u)>>5;
 return taxi_affine_queue(g->queue,g->transport,m,r->position.x,r->position.y,r->position.z,width,height,r->bucket,r->priority,r->flags,mask,tile,g->assets->palette_bases[r->group],r->palette,r->scale,0,0,256,out);
}

int taxi_effect_world_render_owned(TaxiOrdinaryGraphicsOwners *g,const TaxiEffectRecord *record,TaxiPoint camera,int16_t pitch,int16_t yaw,int16_t roll,const TaxiTrig *trig,int32_t focal,int32_t cx,int32_t cy,int32_t phase,int disabled,TaxiEffectRandom random,void *context,uint16_t width,uint16_t height,uint16_t mask,TaxiEffectSprite *request,TaxiStartupObjQueued *out){
 if(!g||!request||!out)return -1;
 TaxiEffectSprite projected;int status=taxi_effect_world_sprite(record,camera,pitch,yaw,roll,trig,focal,cx,cy,phase,disabled,random,context,&projected);if(status<=0)return status;
 *request=projected;return taxi_effect_render_owned(g,&projected,width,height,mask,out);
}
