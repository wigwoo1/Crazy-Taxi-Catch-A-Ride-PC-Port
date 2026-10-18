#include "save_audio_host.h"
int taxi_save_audio_host_platform(void *context,TaxiAudioBootPhase phase,uint32_t rate,uint32_t count){
 TaxiSaveAudioHost *s=context;if(!s||!s->host||rate!=s->rate||!count||count>352||count%4)return -1;
 if(phase==TAXI_AUDIO_BOOT_STOP||phase==TAXI_AUDIO_BOOT_RESET_PREPARE)return taxi_audio_host_stop(s->host);
 if(phase==TAXI_AUDIO_BOOT_RESET_ENABLE)return taxi_audio_host_resume(s->host);
 return -1;
}
int taxi_save_audio_host_deliver(void *context,TaxiFrameWait *wait){
 TaxiSaveAudioHost *s=context;return s?taxi_frame_audio_deliver(s->frame,wait):-1;
}
