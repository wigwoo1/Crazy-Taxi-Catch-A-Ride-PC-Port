#include "audio_output.h"
#include "audio_mix.h"
#include <string.h>
int taxi_audio_output(uint32_t *bank,TaxiAudioOutputBuffer buffers[2],const int16_t *mix,
 size_t capacity,uint8_t processing,uint8_t enabled,uint8_t playing,
 uint8_t effects_enabled,uint8_t effects_active){
 if(!bank||!buffers||*bank>1)return -1;
 TaxiAudioOutputBuffer *out=buffers+*bank;
 int active=processing&&((enabled&&playing)||(effects_enabled&&effects_active));
 if(!out->samples||(active&&(!mix||!out->count||out->count%4||out->count>capacity)))return -1;
 if(active){if(taxi_audio_convert(mix,out->samples,out->count))return -1;}
 else memset(out->samples,0,out->count);
 *bank=1-*bank;return 0;
}
