#include "audio_channel.h"
#include <stdio.h>
int main(void){unsigned seed;while(scanf("%u",&seed)==1){TaxiSoundAsset a={0};TaxiAudioChannel c[4];for(unsigned j=0;j<4;j++)c[j]=(TaxiAudioChannel){.stamp=seed+j,.rate=456,.phase=789,.step=1011,.flags4=1,.flags5=2,.volume=64,.last=7,.sound=&a};taxi_audio_channels_init(c);for(unsigned j=0;j<4;j++)printf("%u %u %u %u %u %u %u %u %u ",c[j].sound!=0,c[j].stamp,(unsigned)c[j].rate,c[j].phase,c[j].step,(unsigned)c[j].flags4,(unsigned)c[j].flags5,(unsigned)c[j].volume,(unsigned)c[j].last);puts("");}return 0;}
