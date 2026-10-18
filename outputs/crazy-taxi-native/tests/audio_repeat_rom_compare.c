#include "audio_asset.h"
#include "audio_channel.h"
#include <stdio.h>
int main(int argc,char **argv){if(argc!=2)return 2;TaxiAudioAsset a={0};if(taxi_audio_asset_load(&a,argv[1]))return 3;unsigned index,count,phase,step,volume;
 while(scanf("%u%u%u%u%u",&index,&count,&phase,&step,&volume)==5){if(index>=203||count>352)return 4;int16_t dst[352];for(unsigned j=0;j<count;j++)dst[j]=(int16_t)(j*257-30000);TaxiAudioChannel c={0};c.sound=a.sounds+index;c.flags4=c.flags5=1;c.phase=phase;c.step=step;
 int n=taxi_audio_channel_mix(&c,dst,count,a.codebook,608,volume);printf("%d %u",n,c.phase);for(unsigned j=0;j<count;j++)printf(" %d",dst[j]);puts("");}taxi_audio_asset_free(&a);return 0;}
