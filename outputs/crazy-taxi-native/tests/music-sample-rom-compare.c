#include "../src/music_event.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void){uint8_t *b=malloc(181241);FILE *f=fopen("outputs/crazy-taxi-native/assets/music/original-module.bin","rb");if(!b||!f||fread(b,1,181241,f)!=181241)return 1;fclose(f);TaxiMusicAsset a;if(taxi_music_asset_index(&a,b,181241))return 1;unsigned sample,volume,seed;
 while(scanf("%u %u %u",&sample,&volume,&seed)==3){TaxiMusicVoice v={0};memset(v.bytes,(uint8_t)seed,32);v.sample=b+a.sample[0].offset;uint8_t event[5]={0,(uint8_t)sample,(uint8_t)volume,0,0};if(!taxi_music_event_prefix(&v,&a,event,0,21024))return 1;for(unsigned j=0;j<28;j++)printf("%02x",v.bytes[j]);printf(" %llu\n",(unsigned long long)(v.sample-b));}free(b);return 0;}
