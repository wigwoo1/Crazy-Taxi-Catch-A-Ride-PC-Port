#ifdef NDEBUG
#undef NDEBUG
#endif
#include "audio_test_render.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
int main(void){
    for(unsigned row=2;row<=3;row++)for(int index=0;index<=(row==2?202:4);index++)
    for(unsigned selected=0;selected<4;selected++)for(unsigned phase=0;phase<64;phase++){
        TaxiAudioTestRender r;
        assert(!taxi_audio_test_render(240,86,(uint8_t)row,(uint8_t)selected,(int16_t)index,phase,&r));
        assert(atoi(r.number)==index+1&&strlen(r.number)==(row==2?3:2));
        assert(r.x==180&&r.y==86&&r.font==1&&r.text_flags==4);
        unsigned expected=selected==row&&(phase&32)?(index!=0)+(index<(row==2?202:4)):0;
        assert(r.arrows==expected);
        for(unsigned i=0;i<r.arrows;i++){
            assert(r.arrow[i].kind==1&&r.arrow[i].flags==1&&r.arrow[i].y==86);
            assert(r.arrow[i].x==180+(r.arrow[i].descriptor==99?-(row==2?22:18):(row==2?14:10)));
        }
    }
    puts("PASS original effect/music test number formats, blink and bounded arrows.");return 0;
}
