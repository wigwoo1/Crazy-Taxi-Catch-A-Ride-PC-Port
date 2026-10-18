#include "../src/music_tracker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void){uint8_t *b=malloc(181241),pitch[1536];FILE *f=fopen("outputs/crazy-taxi-native/assets/music/original-module.bin","rb");if(!b||!f||fread(b,1,181241,f)!=181241)return 1;fclose(f);f=fopen("outputs/crazy-taxi-native/assets/music/pitch-table.bin","rb");if(!f||fread(pitch,1,1536,f)!=1536)return 1;fclose(f);TaxiMusicAsset a;if(taxi_music_asset_index(&a,b,181241))return 1;unsigned pattern,row,seed;
 while(scanf("%u %u %u",&pattern,&row,&seed)==3){TaxiMusicTracker t={0};memset(t.state.state,(uint8_t)seed,12);t.state.state[2]=a.channels;t.state.state[6]=(uint8_t)row;t.state.pattern_offset=a.pattern[pattern].offset;for(unsigned k=0;k<16;k++){memset(t.voice[k].bytes,(uint8_t)seed,32);t.voice[k].sample=b+a.sample[0].offset;}TaxiMusicContext c={.main_mode=1};if(!taxi_music_tracker_row(&t,&a,pitch,21024,&c))return 1;
 for(unsigned j=0;j<12;j++)printf("%02x",t.state.state[j]);printf(" %llu",(unsigned long long)t.state.pattern_offset);for(unsigned k=0;k<4;k++){putchar(' ');for(unsigned j=0;j<28;j++)printf("%02x",t.voice[k].bytes[j]);printf(" %llu",(unsigned long long)(t.voice[k].sample-b));}putchar('\n');}free(b);return 0;}
