#include "game_audio_channels.h"
uint32_t taxi_game_audio_channel_dispatch(void *context,TaxiGameAudioEngineRequest q){
 TaxiGameAudioChannels *s=context;if(!s||s->error)return 0;
 if(q.op==1||q.op==9){
  if(!s->assets||q.handle>=203||!s->output_rate){s->error=-1;return 0;}
  int slot=taxi_audio_channel_start(s->channels,s->assets->sounds+q.handle,s->output_rate,(uint8_t)q.value,(uint8_t)((uint32_t)q.value>>8));
  if(slot==-2)s->error=-2;
  return slot>=0?(uint32_t)slot+1:0;
 }
 if(q.handle<1||q.handle>4){s->error=-3;return 0;}
 TaxiAudioChannel *c=s->channels+q.handle-1;
 if(q.op==2)taxi_audio_channel_volume(c,(uint32_t)q.value);
 else if(q.op==3){if(taxi_audio_channel_pitch(c,(uint32_t)q.value,s->output_rate))s->error=-4;}
 else if(q.op==4)taxi_audio_channel_stop(c,(uint16_t)s->output_rate);
 else if(q.op==6){if(!s->assets||q.value<0||q.value>=203){s->error=-5;return 0;}return (uint32_t)taxi_audio_channel_busy(c,s->assets->sounds+q.value);}
 else s->error=-6;
 return 0;
}
