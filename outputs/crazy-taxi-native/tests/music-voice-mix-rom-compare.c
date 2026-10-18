#include "../src/music_mix.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void word(uint8_t *p,unsigned v){for(unsigned j=0;j<4;j++)p[j]=(uint8_t)(v>>(8*j));}
int main(void){uint8_t *b=malloc(181241);FILE *f=fopen("outputs/crazy-taxi-native/assets/music/original-module.bin","rb");if(!b||!f||fread(b,1,181241,f)!=181241)return 1;fclose(f);TaxiMusicAsset a;if(taxi_music_asset_index(&a,b,181241))return 1;unsigned sample,count,phase,step,volume;int fade;
while(scanf("%u %u %u %u %u %d",&sample,&count,&phase,&step,&volume,&fade)==6){TaxiMusicVoice v={0};memset(v.bytes,165,32);v.sample=b+a.sample[sample].offset;word(v.bytes+12,phase);word(v.bytes+16,step);v.bytes[20]=64;v.bytes[21]=0;v.bytes[22]=64;v.bytes[23]=0;int16_t mixed[352];for(unsigned j=0;j<count;j++)mixed[j]=(int16_t)(j*257-30000);if(taxi_music_voice_mix(&v,&a,mixed,count,(uint8_t)volume,fade!=256,(int16_t)fade)){fprintf(stderr,"mix rejected %u %u %u %u %u %d\n",sample,count,phase,step,volume,fade);return 1;}for(unsigned j=0;j<28;j++)printf("%02x",v.bytes[j]);printf(" %llu",(unsigned long long)(v.sample?(size_t)(v.sample-b):UINT32_MAX));for(unsigned j=0;j<count;j++)printf(" %d",mixed[j]);putchar('\n');}free(b);return 0;}
