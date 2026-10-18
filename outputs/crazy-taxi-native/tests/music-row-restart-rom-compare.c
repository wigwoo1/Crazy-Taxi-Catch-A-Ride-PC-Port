#include "../src/music_tracker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint32_t random_value;static unsigned draws;
static uint32_t random(void *p){(void)p;draws++;return random_value;}
int main(void){uint8_t *bytes=malloc(181241);FILE *f=fopen("outputs/crazy-taxi-native/assets/music/original-module.bin","rb");if(!bytes||!f||fread(bytes,1,181241,f)!=181241)return 1;fclose(f);TaxiMusicAsset a;if(taxi_music_asset_index(&a,bytes,181241))return 1;
 unsigned order,row,pending,mode,value;while(scanf("%u %u %u %u %u",&order,&row,&pending,&mode,&value)==5){TaxiMusicTracker t={0};memset(t.state.state,0xa5,12);TaxiMusicRecord pattern;if(taxi_music_asset_order(&a,(uint8_t)order,&pattern))return 1;t.state.pattern_offset=pattern.offset;t.state.state[5]=(uint8_t)order;t.state.state[6]=(uint8_t)row;t.state.state[10]=(uint8_t)pending;t.state.state[11]=(uint8_t)(pending>>8);TaxiMusicContext c={.main_mode=(uint8_t)mode,.random=random};draws=0;random_value=value;
 if(!taxi_music_tracker_finish_row(&t,&a,&c))return 1;for(unsigned j=0;j<12;j++)printf("%02x",t.state.state[j]);printf(" %llu %u\n",(unsigned long long)t.state.pattern_offset,draws);}free(bytes);return 0;}
