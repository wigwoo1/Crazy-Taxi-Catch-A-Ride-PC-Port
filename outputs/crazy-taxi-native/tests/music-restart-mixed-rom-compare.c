#include "../src/music_tracker.h"
#include <stdio.h>
#include "../src/random.h"
#include <stdlib.h>
#include <string.h>
static uint32_t rng;static unsigned draws;static uint32_t random(void *p){(void)p;draws++;return (uint32_t)taxi_random_range(&rng,0,65535);}
int main(void){uint8_t *b=malloc(181241),pitch[1536];FILE *f=fopen("outputs/crazy-taxi-native/assets/music/original-module.bin","rb");if(!b||!f||fread(b,1,181241,f)!=181241)return 1;fclose(f);f=fopen("outputs/crazy-taxi-native/assets/music/pitch-table.bin","rb");if(!f||fread(pitch,1,1536,f)!=1536)return 1;fclose(f);TaxiMusicAsset a;if(taxi_music_asset_index(&a,b,181241))return 1;unsigned pattern,row,seed,mode,choice,rng_seed;
 while(scanf("%u %u %u %u %u %u",&pattern,&row,&seed,&mode,&choice,&rng_seed)==6){TaxiMusicTracker t={0};memset(t.state.state,(uint8_t)seed,12);t.state.state[0]=1;t.state.playing=1;t.state.state[2]=a.channels;t.state.state[6]=(uint8_t)row;t.state.pattern_offset=a.pattern[pattern].offset;for(unsigned k=0;k<16;k++){memset(t.voice[k].bytes,(uint8_t)seed,32);t.voice[k].sample=b+a.sample[0].offset;}rng=rng_seed;draws=0;TaxiMusicContext c={.main_mode=(uint8_t)mode,.sound_active=(uint8_t)(choice<5),.sound_row=3,.music_selection=(uint8_t)choice,.random=random};if(!taxi_music_tracker_row(&t,&a,pitch,21024,&c))return 1;
 for(unsigned j=0;j<12;j++)printf("%02x",t.state.state[j]);printf(" %llu",(unsigned long long)t.state.pattern_offset);for(unsigned k=0;k<16;k++){putchar(' ');for(unsigned j=0;j<28;j++)printf("%02x",t.voice[k].bytes[j]);printf(" %llu",(unsigned long long)(t.voice[k].sample?(size_t)(t.voice[k].sample-b):UINT32_MAX));}printf(" %u %u %u\n",rng,draws,t.state.playing);}free(b);return 0;}
