#include "audio_asset.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint32_t u32(const uint8_t *p){return p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
void taxi_audio_asset_free(TaxiAudioAsset *a){
    if(!a)return;
    for(unsigned i=0;i<203;i++)free(a->sounds[i].storage);
    memset(a,0,sizeof *a);
}
int taxi_audio_asset_load(TaxiAudioAsset *out,const char *directory){
    if(!out||!directory)return -1;
    TaxiAudioAsset *a=calloc(1,sizeof *a);if(!a)return -1;
    char path[4096];int n=snprintf(path,sizeof path,"%s/codebook.bin",directory);
    if(n<0||(size_t)n>=sizeof path)goto bad;
    FILE *f=fopen(path,"rb");if(!f)goto bad;
    int valid=fread(a->codebook,1,608,f)==608&&fgetc(f)==EOF;fclose(f);if(!valid)goto bad;
    for(unsigned i=0;i<203;i++){
        n=snprintf(path,sizeof path,"%s/%03u.cts",directory,i);
        if(n<0||(size_t)n>=sizeof path)goto bad;
        f=fopen(path,"rb");if(!f)goto bad;
        uint8_t header[8];valid=fread(header,1,8,f)==8;
        if(!valid){fclose(f);goto bad;}
        TaxiSoundAsset *s=a->sounds+i;
        s->rate=(uint16_t)(header[0]|((unsigned)header[1]<<8));
        s->prefix=(uint16_t)(header[2]|((unsigned)header[3]<<8));s->length=u32(header+4);
        if(!s->rate||s->length>2000000||(s->prefix&&s->prefix<(s->length+15)/16)){fclose(f);goto bad;}
        s->size=s->prefix?s->prefix+(s->length+1u)/2:s->length;
        s->storage=malloc(s->size?s->size:1);
        if(!s->storage){fclose(f);goto bad;}
        valid=fread(s->storage,1,s->size,f)==s->size&&fgetc(f)==EOF;fclose(f);
        if(!valid)goto bad;
        if(!s->prefix){
            n=snprintf(path,sizeof path,"%s/%03u.tail",directory,i);
            if(n<0||(size_t)n>=sizeof path)goto bad;
            f=fopen(path,"rb");if(!f)goto bad;
            uint8_t tail[256];valid=fread(tail,1,sizeof tail,f)==sizeof tail&&fgetc(f)==EOF;fclose(f);
            if(!valid)goto bad;
            uint8_t *extended=realloc(s->storage,s->size+sizeof tail);if(!extended)goto bad;
            s->storage=extended;memcpy(s->storage+s->size,tail,sizeof tail);s->size+=sizeof tail;
        }
        if(s->prefix)for(uint32_t block=0;block<(s->length+15)/16;block++)
            if(s->storage[block]>=38)goto bad;
    }
    taxi_audio_asset_free(out);*out=*a;free(a);return 0;
bad:taxi_audio_asset_free(a);free(a);return -1;
}
