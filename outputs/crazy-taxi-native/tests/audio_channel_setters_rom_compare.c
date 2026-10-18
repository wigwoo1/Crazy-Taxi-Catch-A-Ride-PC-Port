#include "audio_channel.h"
#include <stdio.h>
int main(void){unsigned rate,output,volume;while(scanf("%u%u%u",&rate,&output,&volume)==3){TaxiAudioChannel c={0};c.stamp=123;c.phase=456;taxi_audio_channel_volume(&c,volume);if(taxi_audio_channel_pitch(&c,rate,output))return 2;printf("%u %u %u %u %u\n",(unsigned)c.rate,c.step,(unsigned)c.volume,c.stamp,c.phase);}return 0;}
