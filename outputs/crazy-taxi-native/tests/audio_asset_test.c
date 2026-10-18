#ifdef NDEBUG
#undef NDEBUG
#endif
#include "audio_mix.h"
#include "audio_asset.h"
#include "audio_channel.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
static uint8_t *read(const char *path,size_t *size){
    FILE *f=fopen(path,"rb");assert(f);assert(!fseek(f,0,SEEK_END));long length=ftell(f);
    assert(length>=0&&!fseek(f,0,SEEK_SET));*size=(size_t)length;
    uint8_t *bytes=malloc(*size?*size:1);assert(bytes);
    assert(fread(bytes,1,*size,f)==*size&&fgetc(f)==EOF);fclose(f);return bytes;
}
static uint32_t u32(const uint8_t *p){return p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
int main(int argc,char **argv){
    assert(argc==3);char path[1024];size_t code_size,total=0;
    TaxiAudioAsset bank={0};assert(!taxi_audio_asset_load(&bank,argv[1]));
    assert(snprintf(path,sizeof path,"%s/codebook.bin",argv[1])>0);
    uint8_t *codes=read(path,&code_size);assert(code_size==608);
    for(unsigned index=0;index<203;index++){
        size_t size,reference_size;
        snprintf(path,sizeof path,"%s/%03u.cts",argv[1],index);uint8_t *raw=read(path,&size);assert(size>=8);
        unsigned prefix=raw[2]|((unsigned)raw[3]<<8);uint32_t count=u32(raw+4),next=0;
        assert(size==8+(prefix?prefix+(count+1u)/2:count));
        const TaxiSoundAsset *loaded=bank.sounds+index;
        assert(loaded->prefix==prefix&&loaded->length==count&&loaded->size==size-8+(prefix?0:256));
        assert(loaded->rate==(raw[0]|((unsigned)raw[1]<<8))&&!memcmp(loaded->storage,raw+8,size-8));
        if(!prefix){size_t tail_size;snprintf(path,sizeof path,"%s/%03u.tail",argv[1],index);
            uint8_t *tail=read(path,&tail_size);assert(tail_size==256&&!memcmp(loaded->storage+count,tail,256));free(tail);}
        int16_t *decoded=calloc(count?count:1,sizeof *decoded);assert(decoded);
        int status=prefix?taxi_audio_mix_packed(decoded,count,loaded->storage,prefix,loaded->storage+prefix,loaded->size-prefix,bank.codebook,608,0,256,1,&next):
            taxi_audio_mix(decoded,count,loaded->storage,loaded->size,0,256,1,&next);
        assert(!status&&next==count*256u);
        snprintf(path,sizeof path,"%s/%03u.pcm8",argv[2],index);uint8_t *reference=read(path,&reference_size);assert(reference_size==count);
        for(uint32_t i=0;i<count;i++)assert(decoded[i]==(reference[i]<128?(int)reference[i]:(int)reference[i]-256));
        const unsigned rates[3]={10512,15768,21024},frames[3]={176,264,352};
        for(unsigned mode=0;mode<3;mode++){
            TaxiAudioChannel channels[4]={0};
            assert(taxi_audio_channel_start(channels,loaded,rates[mode],0,0)==0);
            unsigned iterations=0;
            while(channels[0].sound){
                assert(++iterations<100000);int16_t buffer[352]={0};
                uint32_t phase=channels[0].phase,step=channels[0].step;
                int mixed=taxi_audio_channel_mix(channels,buffer,frames[mode],bank.codebook,608,64);
                if(mixed<0)fprintf(stderr,"Sound%u mode%u length%u phase%u step%u frame%u status%d\n",index,mode,count,phase,step,frames[mode],mixed);
                assert(mixed>=0&&(unsigned)mixed<=frames[mode]);
                for(unsigned i=0;i<frames[mode];i++){
                    if(i<(unsigned)mixed){
                        uint32_t sample=(phase+i*step)>>8;assert(sample<count);
                        int value=reference[sample]<128?(int)reference[sample]:(int)reference[sample]-256;
                        assert(buffer[i]==value*64);
                    }else assert(!buffer[i]);
                }
                assert(channels[0].phase==phase+(uint32_t)mixed*step);
            }
        }
        total+=count;free(reference);free(decoded);free(raw);
    }
    TaxiAudioAsset saved=bank;assert(taxi_audio_asset_load(&bank,"missing-audio-asset-directory")==-1);
    assert(!memcmp(&bank,&saved,sizeof bank));assert(!memcmp(bank.codebook,codes,608));
    taxi_audio_asset_free(&bank);taxi_audio_asset_free(&bank);
    free(codes);printf("PASS all203 original sounds: %zu byte-exact native decoded samples; transactional load; no host playback claim.\n",total);
}
