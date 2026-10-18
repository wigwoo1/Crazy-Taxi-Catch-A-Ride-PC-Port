#include "music_buffer.h"
#include "music_mix.h"
#include <string.h>
int taxi_music_buffer(TaxiMusicTracker *t,const TaxiMusicAsset *a,int16_t *dst,size_t capacity,
 size_t count,uint8_t enabled,uint8_t effects_enabled,
 uint8_t effects_active,uint8_t processing_enabled,uint8_t volume,int fade_enabled,int16_t fade){
 if(!t||!a||!dst||count>capacity||t->state.state[2]>16)return -1;
 if((enabled&&t->state.playing)||(effects_enabled&effects_active)||!processing_enabled)
  memset(dst,0,count*sizeof(*dst));
 if(enabled&&t->state.playing&&processing_enabled)
  for(unsigned i=0;i<t->state.state[2];i++)
   if(taxi_music_voice_mix(t->voice+i,a,dst,count,volume,fade_enabled,fade))return -1;
 return 0;
}
