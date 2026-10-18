#include "../src/music_tracker.h"
#include "../src/random.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint32_t rng;static unsigned draws;
static uint32_t random_value(void *p){(void)p;draws++;return (uint32_t)taxi_random_range(&rng,0,65535);}
int main(void){uint8_t *b=malloc(181241),pitch[1536];FILE *f=fopen("outputs/crazy-taxi-native/assets/music/original-module.bin","rb");if(!b||!f||fread(b,1,181241,f)!=181241)return 1;fclose(f);f=fopen("outputs/crazy-taxi-native/assets/music/pitch-table.bin","rb");if(!f||fread(pitch,1,1536,f)!=1536)return 1;fclose(f);TaxiMusicAsset a;if(taxi_music_asset_index(&a,b,181241))return 1;
unsigned order,frames,seed,mode;
while(scanf("%u %u %u %u",&order,&frames,&seed,&mode)==4){TaxiMusicTracker t={0};memset(t.state.state,165,12);t.state.state[0]=1;for(unsigned k=0;k<16;k++)memset(t.voice[k].bytes,165,32);if(!taxi_music_tracker_start(&t,&a,(uint8_t)order))return 1;rng=seed;draws=0;uint8_t divider=0;TaxiMusicContext c={.main_mode=(uint8_t)mode,.random=random_value};
for(unsigned frame=0;frame<frames;frame++){uint8_t enabled=(uint8_t)!(frame%193>=100&&frame%193<109);int dispatch=taxi_music_clock_gate(&divider,enabled,t.state.state[0],t.state.playing);if(dispatch&&!taxi_music_tracker_tick(&t,&a,pitch,21024,&c))return 1;printf("%u %u ",divider,dispatch);for(unsigned j=0;j<12;j++)printf("%02x",t.state.state[j]);printf(" %llu",(unsigned long long)t.state.pattern_offset);for(unsigned k=0;k<16;k++){putchar(' ');for(unsigned j=0;j<28;j++)printf("%02x",t.voice[k].bytes[j]);printf(" %llu",(unsigned long long)(t.voice[k].sample?(size_t)(t.voice[k].sample-b):UINT32_MAX));}printf(" %u %u %u\n",rng,draws,t.state.playing);}}
free(b);return 0;}
