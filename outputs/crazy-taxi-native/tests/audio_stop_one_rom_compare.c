#include "audio_channel.h"
#include <stdio.h>
int main(void){unsigned active,stamp,current,volume;
 while(scanf("%u%u%u%u",&active,&stamp,&current,&volume)==4){
  TaxiSoundAsset sound={0};TaxiAudioChannel c={.stamp=0xa5a5a5a5,.rate=(uint16_t)stamp,.phase=1234,.step=5678,.flags4=17,.flags5=19,.volume=(uint8_t)volume,.last=23,.sound=active?&sound:NULL};
  int rc=taxi_audio_channel_stop(&c,(uint16_t)current);
  if(rc<0)return 2;
  printf("%u %u %u %u %u %u %u %u %u\n",c.sound!=NULL,(unsigned)c.rate,c.phase,c.step,c.flags4,c.flags5,c.volume,c.last,c.stamp);
 }return 0;}
