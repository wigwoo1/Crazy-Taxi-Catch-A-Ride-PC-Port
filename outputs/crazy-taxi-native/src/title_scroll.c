#include "title_scroll.h"
#include <string.h>
static int32_t bits(uint32_t v){int32_t s;memcpy(&s,&v,4);return s;}
int taxi_title_scroll_step(TaxiTitleScroll *s,int32_t delta,uint16_t width){
 if(!s||!width||width>512)return -1;
 if(s->timer>33)return 0;
 s->timer=bits((uint32_t)s->timer+(uint32_t)delta);
 /* Original input delta<<11 multiplied by256 before signed /256.
    Ordinary title delta1 is bounded; retain source wrapped32 operations. */
 uint32_t product=((uint32_t)delta<<11)*256u;
 int32_t movement=bits(product)/256;
 s->x_q8=bits((uint32_t)s->x_q8+(uint32_t)movement);
 int32_t limit=(512-(int32_t)width)*256;
 if(s->x_q8<0)s->x_q8=0;
 if(s->x_q8>limit)s->x_q8=limit;
 return 1;
}
