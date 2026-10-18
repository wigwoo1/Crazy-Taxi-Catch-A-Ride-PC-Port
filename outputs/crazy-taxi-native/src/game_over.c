#include "game_over.h"
#include <string.h>
static int32_t trunc8(int32_t value) {return value/8;}
int taxi_game_over_begin(TaxiGameOver *out,unsigned count,unsigned width,const uint16_t measured[16]) {
    if(!out||!measured||!count||count>16||width>32767)return -1;
    TaxiGameOver s={0};s.count=(uint8_t)count;
    /* Source arithmetic shift after subtraction, including negative odd. */
    int32_t gap=(int32_t)width-measured[0];
    s.position[0]=(gap>=0?gap/2:-((-gap+1)/2))*256;
    s.velocity[0]=trunc8(s.position[0]-(int32_t)(width>>1)*256);
    for(unsigned i=1;i<count;i++) {
        s.position[i]=s.position[i-1]+((int32_t)measured[i-1]-measured[i])*256;
        s.velocity[i]=trunc8(s.position[i]-(int32_t)(width>>1)*256);
    }
    s.center=s.current_x=(int32_t)(width>>1)*256;*out=s;return 0;
}
int taxi_game_over_step(TaxiGameOver *s,int32_t delta,int fade_gate) {
    if(!s||fade_gate||s->exiting)return 0;
    if(s->glyph!=s->count) {
        s->current_x+=s->velocity[s->glyph];
        uint16_t bits=(uint16_t)((uint16_t)s->scale+32);memcpy(&s->scale,&bits,2);
        s->steps++;
        if(s->steps==8) {
            s->steps=0;s->glyph++;
            s->current_x=s->center;
            s->scale=0;
        }
        return 0;
    }
    uint16_t bits=(uint16_t)((uint32_t)(uint16_t)s->hold+(uint32_t)delta);
    memcpy(&s->hold,&bits,2);
    if(s->hold>60) {s->exiting=1;return 1;}
    return 0;
}
