#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "music_event.h"
int main(int argc,char **argv){
    assert(argc==3);unsigned char *b=malloc(181241),pitch[1536];assert(b);
    FILE *f=fopen(argv[1],"rb");assert(f);assert(fread(b,1,181241,f)==181241);fclose(f);
    f=fopen(argv[2],"rb");assert(f);assert(fread(pitch,1,1536,f)==1536);fclose(f);
    TaxiMusicAsset a;assert(!taxi_music_asset_index(&a,b,181241));
    for(unsigned sample=1;sample<=31;sample++)for(unsigned volume=0;volume<256;volume++){
        TaxiMusicVoice v;memset(&v,0xa5,sizeof v);v.sample=NULL;
        uint8_t e[5]={0,(uint8_t)sample,(uint8_t)volume,0,0};
        assert(taxi_music_event_prefix(&v,&a,e,pitch,21024));
        assert(v.sample==b+a.sample[sample-1].offset&&v.bytes[8]==sample-1);
        assert(v.bytes[20]==v.sample[14]&&v.bytes[21]==0);
        assert(v.bytes[22]==(volume>=16&&volume<=80?volume-16:64)&&v.bytes[23]==0);
        for(unsigned i=12;i<16;i++)assert(v.bytes[i]==0);
        for(unsigned i=0;i<32;i++)if(i!=8&&(i<12||i>15)&&(i<20||i>23))assert(v.bytes[i]==0xa5);
        TaxiMusicVoice old=v;e[1]=0;e[2]=0;e[0]=128;
        assert(taxi_music_event_prefix(&v,&a,e,pitch,21024)&&!memcmp(&v,&old,sizeof v));
        e[0]=1;assert(taxi_music_event_prefix(&v,&a,e,pitch,21024));
        old=v;e[1]=32;assert(!taxi_music_event_prefix(&v,&a,e,pitch,21024)&&!memcmp(&v,&old,sizeof v));
    }
    for(unsigned parameter=0;parameter<256;parameter++){
        TaxiMusicState s;memset(&s,0xa5,sizeof s);TaxiMusicVoice v;memset(&v,0xa5,sizeof v);v.sample=NULL;
        assert(taxi_music_row_effect(&s,&v,&a,12,(uint8_t)parameter,0));
        assert(v.bytes[22]==parameter&&v.bytes[23]==0);
        assert(taxi_music_row_effect(&s,&v,&a,15,(uint8_t)parameter,0)&&s.state[1]==parameter);
        for(unsigned order=0;order<256;order++){
            s.state[5]=(uint8_t)order;unsigned decimal=(parameter>>4)*10+(parameter&15);
            assert(taxi_music_row_effect(&s,&v,&a,13,(uint8_t)parameter,0));
            assert(s.state[10]==(decimal<=63?decimal:0)&&s.state[11]==0);
            assert(s.state[8]==(order+1<42?order+1:0)&&s.state[9]==0);
            s.state[8]=123;s.state[9]=234;
            assert(taxi_music_row_effect(&s,&v,&a,13,(uint8_t)parameter,1));
            assert(s.state[8]==123&&s.state[9]==234);
        }
    }
    {TaxiMusicState s={0};TaxiMusicVoice v={0};s.state[6]=0;
        assert(taxi_music_row_effect(&s,&v,&a,14,0x60,0));
        for(unsigned i=0;i<4;i++){
            s.state[6]=12;assert(taxi_music_row_effect(&s,&v,&a,14,0x63,0));
            assert(v.bytes[26]==3-i&&s.state[6]==(i==3?12:255));
        }
        TaxiMusicState old=s;TaxiMusicVoice prior=v;
        assert(!taxi_music_row_effect(&s,&v,&a,11,7,0));
        assert(!memcmp(&s,&old,sizeof s)&&!memcmp(&v,&prior,sizeof v));
    }
    free(b);puts("PASS original instrument/volume prefix: all31 samples/all256 volume bytes, no-note retention and invalid guards");return 0;
}
