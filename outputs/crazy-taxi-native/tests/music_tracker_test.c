#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "music_tracker.h"
int main(int argc,char **argv){
    assert(argc==3);uint8_t *b=malloc(181241),pitch[1536];assert(b);
    FILE *f=fopen(argv[1],"rb");assert(f);assert(fread(b,1,181241,f)==181241);fclose(f);
    f=fopen(argv[2],"rb");assert(f);assert(fread(pitch,1,1536,f)==1536);fclose(f);
    TaxiMusicAsset a;assert(!taxi_music_asset_index(&a,b,181241));
    for(unsigned initial=0;initial<256;initial++)for(unsigned mask=0;mask<8;mask++){
        uint8_t divider=(uint8_t)initial;
        int dispatched=taxi_music_clock_gate(&divider,mask&1,mask&2,mask&4);
        assert(divider==(initial==6?0:(uint8_t)(initial+1)));
        assert(dispatched==(initial!=6&&mask==7));
    }
    uint8_t divider=0;unsigned dispatches=0;
    for(unsigned i=0;i<700;i++)dispatches+=taxi_music_clock_gate(&divider,1,1,1);
    assert(divider==0&&dispatches==600);
    const uint8_t orders[5]={63,88,115,34,5};unsigned long nonzero=0;unsigned total_dispatches=0;
    for(unsigned choice=0;choice<5;choice++){
        TaxiMusicTracker t={0};TaxiMusicContext c={.main_mode=1};
        assert(taxi_music_tracker_start(&t,&a,orders[choice]));
        uint8_t clock=0;
        for(unsigned tick=0;tick<10000;tick++){
            int run=taxi_music_clock_gate(&clock,1,1,t.state.playing);
            total_dispatches+=(unsigned)run;
            if(run&&!taxi_music_tracker_tick(&t,&a,pitch,21024,&c)){
                fprintf(stderr,"tracker dependency failure choice%u tick%u order%u row%u\n",choice,tick,t.state.state[5],t.state.state[6]);return 1;
            }
            int16_t dst[352]={0};
            for(unsigned v=0;v<4;v++)assert(!taxi_music_voice_mix(t.voice+v,&a,dst,352,64,0,0));
            for(unsigned i=0;i<352;i++)if(dst[i])nonzero++;
        }
    }
    assert(nonzero&&total_dispatches==42860);printf("PASS source music tracker: five starts/50000 source frames/%u gated dispatches, %lu nonzero samples\n",total_dispatches,nonzero);
    free(b);return 0;
}
