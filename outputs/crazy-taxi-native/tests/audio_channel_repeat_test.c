#ifdef NDEBUG
#undef NDEBUG
#endif
#include "audio_channel.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void){uint8_t bytes[]={1,2,3,4,128,129};TaxiSoundAsset a={10512,0,4,bytes,6};TaxiAudioChannel c={0};c.sound=&a;c.flags4=c.flags5=1;c.step=256;c.volume=64;int16_t dst[12]={0};
 assert(taxi_audio_channel_mix(&c,dst,12,0,0,1)==12);const int16_t expect[]={1,2,3,4,-128,2,3,4,-128,2,3,4};assert(!memcmp(dst,expect,sizeof dst));assert(c.phase==1024&&c.sound==&a&&c.flags4==1&&c.flags5==1);
 a.prefix=1;TaxiAudioChannel old=c;int16_t before[12];memcpy(before,dst,sizeof dst);assert(!taxi_audio_channel_mix(&c,dst,12,0,0,1));assert(!memcmp(&c,&old,sizeof c)&&!memcmp(dst,before,sizeof dst));
 a.prefix=0;a.size=4;c.phase=0;old=c;assert(taxi_audio_channel_mix(&c,dst,12,0,0,1)==-2);assert(!memcmp(&c,&old,sizeof c)&&!memcmp(dst,before,sizeof dst));
 a.size=6;c.phase=256;c.step=0;assert(taxi_audio_channel_mix(&c,dst,3,0,0,1)==3&&c.phase==256);assert(dst[0]==before[0]+2);
 puts("PASS protected raw repeat, source tail sample, retained owner, packed skip, atomic bounds and zero step");return 0;}
