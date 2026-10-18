#include "traffic_sprite.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv){
    assert(argc==3);TaxiTrafficSprites *s=malloc(sizeof *s);assert(s);
    assert(taxi_traffic_sprites_load(s,argv[1])==0);
    TaxiTrafficSprites *saved=malloc(sizeof *saved);assert(saved);*saved=*s;
    assert(taxi_traffic_sprites_load(s,"missing-traffic.ctb")==-1&&memcmp(s,saved,sizeof *s)==0);
    FILE *f=fopen(argv[1],"rb");assert(f&&fseek(f,12,SEEK_SET)==0);
    for(unsigned i=0;i<256;i++){int low=fgetc(f),high=fgetc(f);assert(s->palette[i]==low+256*high);}
    for(size_t i=0;i<sizeof s->pixels;i++)assert(s->pixels[i]==fgetc(f));
    assert(fgetc(f)==EOF);fclose(f);
    uint8_t rgba[128*64*4];uint16_t display[256];
    for(unsigned i=0;i<256;i++)display[i]=(uint16_t)((i&31)|((i&31)<<5)|((i&31)<<10));
    for(unsigned frame=0;frame<16;frame++){
        assert(taxi_traffic_sprite_rgba(s,frame,display,rgba)==0);
        for(unsigned i=0;i<128*64;i++){
            unsigned index=s->pixels[frame*8192+i],value=index&31;
            assert(rgba[4*i]==(value*8+value/4)&&rgba[4*i+1]==rgba[4*i]&&rgba[4*i+2]==rgba[4*i]);
            assert(rgba[4*i+3]==(index?255:0));
        }
    }
    memset(rgba,17,sizeof rgba);assert(taxi_traffic_sprite_rgba(s,16,display,rgba)==-1);
    for(unsigned i=0;i<sizeof rgba;i++)assert(rgba[i]==17);
    f=fopen(argv[2],"rb");assert(f);
    for(unsigned i=0;i<256;i++){
        int low=fgetc(f),high=fgetc(f);assert(low>=0&&high>=0);
        display[i]=(uint16_t)(low+256*high);
    }
    assert(fgetc(f)==EOF);fclose(f);
    for(unsigned frame=0;frame<16;frame++){
        assert(taxi_traffic_sprite_rgba(s,frame,display,rgba)==0);
        for(unsigned i=0;i<8192;i++){
            unsigned index=s->pixels[frame*8192+i];
            for(unsigned channel=0;channel<3;channel++){
                unsigned value=(display[index]>>(5*channel))&31;
                assert(rgba[i*4+channel]==value*8+value/4);
            }
            assert(rgba[i*4+3]==(index?255:0));
        }
    }
    for(int camera=0;camera<4096;camera++)for(int actor=0;actor<4096;actor++){
        int direction=-1024;unsigned expected=(unsigned)((camera+actor+direction+128+8192)%4096)/256;
        assert(taxi_traffic_sprite_frame(camera,actor,direction)==expected);
    }
    free(saved);free(s);puts("Original traffic bitmap bytes and directional frame selection PASS");return 0;
}
