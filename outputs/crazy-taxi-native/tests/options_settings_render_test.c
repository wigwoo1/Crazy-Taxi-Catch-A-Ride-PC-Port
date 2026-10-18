#ifdef NDEBUG
#undef NDEBUG
#endif
#include "options_settings_render.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
    TaxiGameSettings s={{0}};TaxiOptionsSettingsRender r;
    for(unsigned value=0;value<256;value++)for(unsigned selected=0;selected<5;selected++)for(unsigned phase=0;phase<64;phase++){
        s.bytes[5]=(uint8_t)value;
        assert(!taxi_options_settings_render(240,46,0,(uint8_t)selected,&s,phase,&r));
        char number[4];snprintf(number,sizeof number,"%u",value);
        assert(!strcmp(number,r.time.number)&&r.time.x==180&&r.time.y==46&&r.time.font==1&&r.time.text_flags==4&&!r.bars);
        unsigned count=0;
        if(!selected&&(phase&32)){
            if(value!=35){assert(r.time.arrow[count].descriptor==99&&r.time.arrow[count].x==162);count++;}
            if(value!=70){assert(r.time.arrow[count].descriptor==100&&r.time.arrow[count].x==190);count++;}
        }
        assert(r.time.arrows==count);
    }
    for(unsigned value=0;value<256;value++)for(unsigned row=1;row<=2;row++){
        s.bytes[3]=(uint8_t)value;unsigned n=row==1?value%16:value/16;
        assert(!taxi_options_settings_render(240,62,(uint8_t)row,0,&s,32,&r));
        assert(r.bars==2&&!r.time.arrows&&!r.time.number[0]);
        assert(r.bar[0].descriptor==6+(n<4?n:4));
        unsigned rest=n>4?n-4:0;assert(r.bar[1].descriptor==6+(rest<4?rest:4));
        for(unsigned i=0;i<2;i++)assert(r.bar[i].kind==3&&r.bar[i].flags==1&&r.bar[i].x==148+(int)i*32&&r.bar[i].y==62);
    }
    puts("PASS original Options visuals: unpadded Time, source blink/limits/arrow anchors and both settings nibbles mapped to source bar cells.");
    return 0;
}
