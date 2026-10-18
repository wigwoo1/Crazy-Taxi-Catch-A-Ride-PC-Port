#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "music_state.h"
int main(int argc,char **argv){
    assert(argc==2);FILE *f=fopen(argv[1],"rb");assert(f);
    unsigned char *b=malloc(181241);assert(b);
    assert(fread(b,1,181241,f)==181241);assert(fgetc(f)==EOF);fclose(f);
    TaxiMusicAsset a;assert(!taxi_music_asset_index(&a,b,181241));
    for(unsigned order=0;order<256;order++){
        TaxiMusicState s;memset(&s,0xa5,sizeof s);
        TaxiMusicRecord p;if(taxi_music_asset_order(&a,(uint8_t)order,&p)!=0){ TaxiMusicState old=s; assert(!taxi_music_start(&s,&a,(uint8_t)order)); assert(!memcmp(&s,&old,sizeof s)); continue; }
        assert(taxi_music_start(&s,&a,(uint8_t)order));
        assert(s.playing==1&&s.pattern_offset==p.offset);
        assert(s.state[1]==6&&s.state[2]==4&&s.state[5]==order);
        for(unsigned j=0;j<12;j++){
            unsigned expected=j==1?6:j==2?4:j==5?order:
                j>=8?255:j==0||j==7?0xa5:0;
            assert(s.state[j]==expected);
        }
        for(unsigned i=0;i<16;i++)for(unsigned j=0;j<32;j++)
            assert(s.voice[i][j]==(j==0||j==2||j==3||j==8||j==9||j>=28?0:0xa5));
        TaxiMusicState old=s;assert(!taxi_music_start(&s,NULL,0));
        assert(!memcmp(&s,&old,sizeof s));
    }
    for(unsigned speed=0;speed<256;speed++)for(unsigned delay=0;delay<256;delay++)for(unsigned tick=0;tick<256;tick++){
        TaxiMusicState s;memset(&s,0xa5,sizeof s);
        s.state[1]=(uint8_t)speed;s.state[4]=(uint8_t)delay;s.state[3]=(uint8_t)tick;
        TaxiMusicState expected=s;unsigned next=(tick+1)&255;
        if(next>=speed+delay){expected.state[3]=0;expected.state[4]=0;}
        else expected.state[3]=(uint8_t)next;
        taxi_music_tick_finish(&s);assert(!memcmp(&s,&expected,sizeof s));
    }
    for(unsigned pending=0;pending<65536;pending++){
        TaxiMusicState s;memset(&s,0xa5,sizeof s);assert(taxi_music_start(&s,&a,63));
        s.state[8]=(uint8_t)pending;s.state[9]=(uint8_t)(pending>>8);
        s.state[10]=(uint8_t)pending;s.state[11]=(uint8_t)(pending>>8);
        TaxiMusicState old=s;TaxiMusicRecord p;
        int valid=pending>=32768||taxi_music_asset_order(&a,(uint8_t)pending,&p)==0;
        assert(taxi_music_row_prepare(&s,&a)==valid);
        if(!valid||pending>=32768)assert(!memcmp(&s,&old,sizeof s));
        else assert(s.state[5]==(uint8_t)pending&&s.state[6]==(uint8_t)pending&&s.pattern_offset==p.offset);
    }
    for(unsigned order=0;order<127;order++){
        TaxiMusicState s;memset(&s,0xa5,sizeof s);assert(taxi_music_start(&s,&a,(uint8_t)order));
        unsigned rows=b[s.pattern_offset];
        for(unsigned row=0;row<rows;row++){
            s.state[6]=(uint8_t)row;s.state[10]=255;s.state[11]=255;
            int end=taxi_music_row_finish(&s,&a);
            assert(end==(row+1==rows&&order+1==127));
            assert(s.state[10]==(row+1==rows?0:row+1)&&s.state[11]==0);
            if(row+1==rows)assert(s.state[8]==order+1&&s.state[9]==0);
        }
        s.state[10]=17;s.state[11]=0;TaxiMusicState old=s;
        assert(taxi_music_row_finish(&s,&a)==0&&!memcmp(&s,&old,sizeof s));
    }
    free(b);puts("Original tracker start and tick finish: orders, voice retention, all byte counter/speed/delay combinations PASS");return 0;
}

