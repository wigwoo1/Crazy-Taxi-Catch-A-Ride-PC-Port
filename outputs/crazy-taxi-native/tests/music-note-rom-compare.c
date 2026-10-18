#include "../src/music_event.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void){uint8_t *b=malloc(181241),pitch[1536];FILE *f=fopen("outputs/crazy-taxi-native/assets/music/original-module.bin","rb");if(!b||!f||fread(b,1,181241,f)!=181241)return 1;fclose(f);f=fopen("outputs/crazy-taxi-native/assets/music/pitch-table.bin","rb");if(!f||fread(pitch,1,1536,f)!=1536)return 1;fclose(f);TaxiMusicAsset a;if(taxi_music_asset_index(&a,b,181241))return 1;unsigned sample,note,rate,seed;
 while(scanf("%u %u %u %u",&sample,&note,&rate,&seed)==4){TaxiMusicVoice v={0};memset(v.bytes,(uint8_t)seed,32);v.sample=b+a.sample[sample].offset;uint8_t event[5]={(uint8_t)note,0,0,0,0};if(!taxi_music_event_prefix(&v,&a,event,pitch,rate))return 1;for(unsigned j=0;j<28;j++)printf("%02x",v.bytes[j]);putchar('\n');}free(b);return 0;}
