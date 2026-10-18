#include "effect_presentation_interpolation.h"
#include "render_interpolation.h"
#include <math.h>
#include <string.h>
static int32_t signed_bits(uint32_t v){int32_t s;memcpy(&s,&v,4);return s;}
TaxiEffectSprite taxi_effect_presentation_interpolate(const TaxiEffectPresentationSlot *a,const TaxiEffectPresentationSlot *b,uint64_t ag,uint64_t bg,double alpha,int *blended){
 if(blended)*blended=0;TaxiEffectSprite out=b->sprite;
 if(!a->visible||!b->visible||!ag||ag!=bg||a->record.bytes[52]!=b->record.bytes[52]||!isfinite(alpha)||alpha<0||alpha>=1)return out;
 TaxiPoint base=taxi_render_point(a->unjittered.position,b->unjittered.position,alpha);
 out.position.x=signed_bits((uint32_t)base.x+(uint32_t)b->sprite.position.x-(uint32_t)b->unjittered.position.x);
 out.position.y=signed_bits((uint32_t)base.y+(uint32_t)b->sprite.position.y-(uint32_t)b->unjittered.position.y);
 out.position.z=base.z;
 out.scale=(int16_t)((double)a->sprite.scale+((double)b->sprite.scale-a->sprite.scale)*alpha);
 if(blended)*blended=1;return out;
}
