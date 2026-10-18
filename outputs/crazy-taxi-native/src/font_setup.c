#include "font_setup.h"
static void word(uint8_t *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(8*i));}
int taxi_font_setup(TaxiFontSetup *s,uint32_t slot,uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e){
 if(!s)return -1;unsigned index=slot&255;
 if(index<=3){uint8_t *p=s->descriptors[index];word(p,e);word(p+4,b);word(p+8,a);word(p+12,c);word(p+16,d);p[32]=1;}
 s->cursor=0;return 0;
}
int taxi_font_disable(TaxiFontSetup *s,uint32_t slot){
 if(!s)return -1;if((slot&255)<=3)s->descriptors[slot&255][32]=0;return 0;
}
