#include "display_mode.h"
static void half(uint8_t *p,uint16_t v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
static void word(uint8_t *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(8*i));}
int taxi_display_mode_four(TaxiDisplayMode *s,uint32_t mode,uint32_t sprites){
 if(!s||(mode&255)!=4)return -1;
 s->globals[8]=0;s->globals[24]=0;s->globals[25]=0;s->globals[1]=4;
 half(s->globals+2,240);half(s->globals+4,160);half(s->globals+6,8);
 word(s->globals+12,0x06000000);word(s->globals+16,0x0600a000);
 s->control=(sprites&255)?0x1444:0x0444;s->bg2_control|=0x00c2;s->mosaic=0;s->sprite_request_7e38=0;return 0;
}
