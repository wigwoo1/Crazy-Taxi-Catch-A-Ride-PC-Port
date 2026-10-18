#ifdef NDEBUG
#undef NDEBUG
#endif
#include "audio_volume_render.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
static uint8_t canvas[153600],indices[1280];
int main(int argc,char **argv){
    for(unsigned volume=0;volume<256;volume++){
        TaxiAudioVolumeSprite sprites[2];
        assert(!taxi_audio_volume_render(240,54,(uint8_t)volume,sprites));
        unsigned left=volume/8;if(left>4)left=4;
        unsigned right=volume/8<4?0:volume/8-4;if(right>4)right=4;
        assert(sprites[0].descriptor==left+6&&sprites[1].descriptor==right+6);
        assert(sprites[0].x==148&&sprites[1].x==180);
        for(unsigned i=0;i<2;i++)assert(sprites[i].kind==3&&sprites[i].flags==1&&sprites[i].y==54);
    }
    assert(taxi_audio_volume_render(240,0,64,0)==-1);
    uint16_t palette[16]={0,31,31<<5};
    memset(indices,0,sizeof indices);indices[4*256]=1;indices[4*256+1]=2;
    TaxiAudioVolumeSprite sprites[2];assert(!taxi_audio_volume_render(240,54,64,sprites));
    memset(canvas,17,sizeof canvas);
    assert(!taxi_audio_volume_compose(canvas,indices,palette,sprites));
    if(argc==2){
        char path[4096];uint8_t reference[5120],raw_palette[32];
        snprintf(path,sizeof path,"%s/volume.indices",argv[1]);FILE *file=fopen(path,"rb");assert(file);
        assert(fread(indices,1,sizeof indices,file)==sizeof indices&&fgetc(file)==EOF);fclose(file);
        snprintf(path,sizeof path,"%s/volume.rgb555",argv[1]);file=fopen(path,"rb");assert(file);
        assert(fread(raw_palette,1,32,file)==32&&fgetc(file)==EOF);fclose(file);
        for(unsigned i=0;i<16;i++)palette[i]=(uint16_t)(raw_palette[i*2]|raw_palette[i*2+1]<<8);
        snprintf(path,sizeof path,"%s/volume.rgba",argv[1]);file=fopen(path,"rb");assert(file);
        assert(fread(reference,1,sizeof reference,file)==sizeof reference&&fgetc(file)==EOF);fclose(file);
        for(unsigned descriptor=6;descriptor<=10;descriptor++){
            sprites[0]=(TaxiAudioVolumeSprite){(uint8_t)descriptor,3,1,0,0};
            sprites[1]=(TaxiAudioVolumeSprite){(uint8_t)descriptor,3,1,32,0};
            memset(canvas,17,sizeof canvas);assert(!taxi_audio_volume_compose(canvas,indices,palette,sprites));
            for(unsigned y=0;y<8;y++)for(unsigned x=0;x<32;x++)for(unsigned part=0;part<2;part++){
                const uint8_t *expected=reference+((descriptor-6)*256+y*32+x)*4;
                const uint8_t *actual=canvas+(y*240+x+part*32)*4;
                for(unsigned c=0;c<4;c++)assert(actual[c]==(expected[3]?expected[c]:17));
            }
        }
    }
    memset(palette,0,sizeof palette);palette[1]=31;palette[2]=31<<5;
    memset(indices,0,sizeof indices);indices[4*256]=1;indices[4*256+1]=2;
    assert(!taxi_audio_volume_render(240,54,64,sprites));memset(canvas,17,sizeof canvas);
    assert(!taxi_audio_volume_compose(canvas,indices,palette,sprites));
    assert(canvas[(54*240+148)*4]==255&&canvas[(54*240+148)*4+1]==0);
    assert(canvas[(54*240+149)*4]==0&&canvas[(54*240+149)*4+1]==255);
    assert(canvas[(54*240+150)*4]==17&&canvas[(54*240+180)*4]==255);
    indices[0]=16;assert(taxi_audio_volume_compose(canvas,indices,palette,sprites)==-1);
    assert(canvas[(54*240+150)*4]==17);indices[0]=0;
    sprites[0].x=INT_MAX;sprites[1].x=INT_MIN;sprites[0].y=sprites[1].y=INT_MAX;
    assert(!taxi_audio_volume_compose(canvas,indices,palette,sprites));
    puts("PASS original audio volume sprite descriptors and two-part layout for all256 volume bytes.");
    return 0;
}
