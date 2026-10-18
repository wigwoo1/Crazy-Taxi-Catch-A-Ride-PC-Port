#include "../src/music_event.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void){uint8_t *b=malloc(181241);FILE *f=fopen("outputs/crazy-taxi-native/assets/music/original-module.bin","rb");if(!b||!f||fread(b,1,181241,f)!=181241)return 1;fclose(f);TaxiMusicAsset a;if(taxi_music_asset_index(&a,b,181241))return 1;unsigned command,param,seed;
 while(scanf("%u %u %u",&command,&param,&seed)==3){TaxiMusicState s={0};TaxiMusicVoice v={0};memset(s.state,(uint8_t)seed,12);memset(v.bytes,(uint8_t)seed,32);s.state[2]=1;s.state[6]=(uint8_t)(seed?63:0);s.state[8]=s.state[9]=s.state[10]=s.state[11]=255;
 if(!taxi_music_row_effect(&s,&v,&a,(uint8_t)command,(uint8_t)param,0))return 1;
 for(unsigned j=0;j<12;j++)printf("%02x",s.state[j]);putchar(' ');for(unsigned j=0;j<32;j++)printf("%02x",v.bytes[j]);putchar('\n');}free(b);return 0;}
