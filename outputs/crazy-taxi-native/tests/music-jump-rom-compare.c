#include "../src/music_tracker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned draws,value;static uint32_t random(void *p){(void)p;draws++;return value;}
int main(void){uint8_t *b=malloc(181241);FILE *f=fopen("outputs/crazy-taxi-native/assets/music/original-module.bin","rb");if(!b||!f||fread(b,1,181241,f)!=181241)return 1;fclose(f);TaxiMusicAsset a;if(taxi_music_asset_index(&a,b,181241))return 1;unsigned mode,active,row,choice,param,random_value;
 while(scanf("%u %u %u %u %u %u",&mode,&active,&row,&choice,&param,&random_value)==6){TaxiMusicTracker t={0};memset(t.state.state,165,12);t.state.pattern_offset=0x106;t.state.playing=165;TaxiMusicContext c={.main_mode=(uint8_t)mode,.sound_active=(uint8_t)active,.sound_row=(uint8_t)row,.music_selection=(uint8_t)choice,.random=random};draws=0;value=random_value;if(!taxi_music_tracker_jump(&t,&a,&c,(uint8_t)param))return 1;
 for(unsigned j=1;j<12;j++)printf("%02x",t.state.state[j]);printf(" %llu %u %u\n",(unsigned long long)t.state.pattern_offset,draws,t.state.playing);}free(b);return 0;}
