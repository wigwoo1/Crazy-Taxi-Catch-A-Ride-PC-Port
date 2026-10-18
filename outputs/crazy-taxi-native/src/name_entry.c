#include "name_entry.h"
#include <stdio.h>
#include <string.h>
int taxi_name_ring_glyph(const TaxiNameLayout *layout,uint8_t selected,
    int16_t angle,unsigned slot,uint16_t height,TaxiNameRingGlyph *out) {
    if(!layout||!out||selected>42||slot>15)return -1;
    TaxiNameRingGlyph g={0};
    int character=(int)slot+selected-8;
    while(character<0)character+=43;
    while(character>42)character-=43;
    g.character=(uint8_t)character;
    g.x=layout->ring[slot]+angle;g.y=(int32_t)height-16;
    int32_t lookup=g.x<0?0:(g.x>239?239:g.x);
    int scale=256;
    if(lookup>=88&&lookup<=152) {
        int distance=lookup>120?lookup-120:120-lookup;
        scale=512-distance*8;
    }
    if(scale==256) {
        g.x-=8;g.y-=8;g.scale=256;g.visible=1;
    } else if(slot>=7&&slot<=9) {
        g.scale=(int16_t)(scale<257?257:scale);g.affine=1;g.visible=1;
    }
    *out=g;return 0;
}
int taxi_name_entry_eligible(uint8_t settings,uint8_t rank) {
    return !(settings&224)&&(uint8_t)(rank-1)<=9;
}
int taxi_name_characters_load(uint8_t output[43],const char *path) {
    if(!output||!path)return -1;
    FILE *f=fopen(path,"rb");if(!f)return -1;
    uint8_t b[47];int ok=fread(b,1,47,f)==47&&fgetc(f)==EOF&&!ferror(f)&&!memcmp(b,"CTN1",4);
    fclose(f);if(!ok)return -1;
    memcpy(output,b+4,43);return 0;
}
int taxi_name_layout_load(TaxiNameLayout *out,const char *path) {
    if(!out||!path)return -1;FILE *f=fopen(path,"rb");if(!f)return -1;
    uint8_t bytes[60];int ok=fread(bytes,1,60,f)==60&&fgetc(f)==EOF&&!ferror(f)&&!memcmp(bytes,"CNL1",4);fclose(f);if(!ok)return -1;
    TaxiNameLayout t;
    for(unsigned i=0;i<16;i++) {uint16_t value=(uint16_t)(bytes[4+i*2]|bytes[5+i*2]<<8);memcpy(t.ring+i,&value,2);}
    for(unsigned i=0;i<6;i++) {
        const uint8_t *p=bytes+36+i*4;uint32_t value=p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
        memcpy(&t.slots[i/2][i%2],&value,4);
    }
    *out=t;return 0;
}
void taxi_name_entry_begin(TaxiNameEntry *s,const TaxiNameLayout *layout,uint16_t height) {
    memset(s,0,sizeof *s);memcpy(s->slots,layout->slots,sizeof s->slots);s->screen_height=height;
}
static void advance(TaxiNameEntry *s) {
    int selected=s->selection+(s->move==-1?1:-1);
    s->selection=(uint8_t)(selected<0?42:(selected==43?0:selected));s->angle=0;s->move=0;
}
unsigned taxi_name_entry_step(TaxiNameEntry *s,const uint8_t chars[43],
    int left,int right,int confirm,int back,int fade_gate) {
    if(fade_gate||s->exiting)return 0;
    if(s->automatic) {
        if(!s->move) {s->move=s->automatic;return 0;}
        s->angle=(int16_t)(s->angle+s->move*2);
        if(s->angle>=-16&&s->angle<=16)return 0;
        advance(s);if(s->selection==42)s->automatic=0;return 0;
    }
    if(s->move) {
        s->angle=(int16_t)(s->angle+s->move*2);
        if(s->angle>=-16&&s->angle<=16)return 0;
        advance(s);return 0;
    }
    if(s->flying) {
        s->flight_x+=s->velocity_x;s->flight_y+=s->velocity_y;
        s->flight_ticks++;
        if(s->flight_ticks!=16)return 0;
        s->flight_ticks=0;s->flying=0;
        s->flight_x=s->target_x;s->flight_y=s->target_y;
        if(s->length<3)s->name[s->length++]=(char)chars[s->selection];
        if(s->length==3)s->automatic=s->selection>21?-1:1;
        return TAXI_NAME_ACCEPT;
    }
    if(left) {s->move=-1;return TAXI_NAME_MOVE;}
    if(right) {s->move=1;return TAXI_NAME_MOVE;}
    if(confirm) {
        if(s->selection==42) {s->exiting=1;return TAXI_NAME_FADE_OUT;}
        if(s->length<3) {
            s->flying=1;s->flight_ticks=0;
            s->target_x=s->slots[s->length][0]*256;s->target_y=s->slots[s->length][1]*256;
            s->flight_x=120*256;s->flight_y=((int32_t)s->screen_height-16)*256;
            s->velocity_x=(s->target_x-s->flight_x)/16;s->velocity_y=(s->target_y-s->flight_y)/16;
            return 0;
        }
    }
    if(back&&s->length) {s->name[--s->length]=0;return TAXI_NAME_DELETE;}
    return 0;
}
