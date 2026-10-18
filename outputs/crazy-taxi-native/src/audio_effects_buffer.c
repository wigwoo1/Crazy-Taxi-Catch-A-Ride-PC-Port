#include "audio_effects_buffer.h"
int taxi_audio_effects_buffer(TaxiAudioChannel c[4],uint8_t *request,int16_t *mix,
 size_t count,const uint8_t *codebook,size_t codes,uint8_t enabled,
 uint8_t active,uint8_t processing,uint8_t global){
 if(!c||!request||(!mix&&count))return -1;
 *request=0;
 if(!enabled||!active||!processing)return 0;
 for(unsigned i=0;i<4;i++)if(c[i].sound){
  *request=(uint8_t)(*request+1);
  if(taxi_audio_channel_mix(c+i,mix,count,codebook,codes,
    taxi_audio_effective_volume(global,c[i].volume))<0)return -1;
 }
 return 0;
}
