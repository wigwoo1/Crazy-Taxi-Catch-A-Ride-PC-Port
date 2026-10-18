#include "audio_frame.h"
#include "music_buffer.h"
#include "audio_effects_buffer.h"
int taxi_audio_frame_step(TaxiAudioFrame *s,TaxiMusicTracker *t,const TaxiMusicAsset *m,
 const uint8_t *pitch,const TaxiMusicContext *context,TaxiAudioChannel c[4],
 const TaxiAudioAsset *a,int16_t *mix,size_t capacity,TaxiAudioOutputBuffer out[2],int *dispatched){
 if(!s||!t||!m||!pitch||!context||!c||!a||!mix||!out||!dispatched||s->bank>1||!s->rate)return -1;
 size_t count=out[s->bank].count;
 if(count>capacity)return -1;
 *dispatched=taxi_music_clock_gate(&s->divider,s->music_enabled,t->state.state[0],t->state.playing);
 if(*dispatched&&!taxi_music_tracker_tick(t,m,pitch,s->rate,context))return -1;
 if(taxi_music_buffer(t,m,mix,capacity,count,s->music_enabled,s->effects_enabled,
  s->effects_active,s->processing,s->music_volume,taxi_music_fade_gate(s->ended,s->outer),s->fade))return -1;
 s->music_nonzero=0;s->music_mixed=(uint8_t)(s->music_enabled&&t->state.playing&&s->processing);
 if(s->music_mixed)for(size_t i=0;i<count;i++)if(mix[i])s->music_nonzero++;
 if(taxi_audio_effects_buffer(c,&s->request_756c,mix,count,a->codebook,608,
  s->effects_enabled,s->effects_active,s->processing,s->effects_volume))return -1;
 return taxi_audio_output(&s->bank,out,mix,capacity,s->processing,
  s->music_enabled,t->state.playing,s->effects_enabled,s->effects_active);
}
