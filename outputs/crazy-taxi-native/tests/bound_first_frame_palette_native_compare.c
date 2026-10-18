#include "city_palette_owned.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
static uint16_t half(const uint8_t *p){return (uint16_t)(p[0]|(unsigned)p[1]<<8);}
static uint32_t word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static void put_half(uint8_t *p,uint16_t v){p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
static void put_word(uint8_t *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(i*8));}
int main(int argc,char **argv){if(argc!=4)return 1;FILE *f=fopen(argv[1],"rb"),*r=fopen(argv[2],"rb");if(!f||!r)return 2;uint8_t state[5178];if(fread(state,1,sizeof state,f)!=sizeof state)return 3;fclose(f);uint8_t *rom=malloc(8388608);if(!rom||fread(rom,1,8388608,r)!=8388608)return 4;fclose(r);
 TaxiPaletteRegistration registry={.count=half(state+16),.bg_bytes=half(state+18),.obj_bytes=half(state+20),.flags=state[0]};if(registry.count>128)return 5;memcpy(registry.records,state+32,2560);
 TaxiPaletteFadeStart fade={0};fade.masks=(TaxiPaletteFadeSetup){.background_mask=half(state+22),.object_mask=half(state+24),.mode=state[3],.cap=(uint8_t)half(state+28),.endpoint_count=state[4]};
 fade.fade=(TaxiPaletteFade){.progress_178a=(int16_t)half(state+26),.mode_1773=state[3],.active_177b=state[11],.endpoint_count_1774=state[4],.interval_1771=state[1],.reload_1772=state[2]};fade.scale_x_177c=half(state+12);fade.scale_y_177e=half(state+14);
 for(unsigned i=0;i<3;i++){fade.motion.position[i]=(int32_t)word(state+2592+i*4);fade.motion.velocity[i]=(int32_t)word(state+2604+i*4);}
 TaxiPaletteBanks banks;memcpy(&banks,state+4152,1024);uint16_t mosaic=half(state+5176);TaxiCityPaletteOwners owner={&registry,&banks,&fade,rom,8388608,240,160};int delta=(int)strtol(argv[3],NULL,10);if(taxi_city_palette_frame(&owner,delta,&mosaic))return 6;
 state[1]=fade.fade.interval_1771;state[2]=fade.fade.reload_1772;state[3]=fade.fade.mode_1773;state[4]=fade.fade.endpoint_count_1774;state[11]=fade.fade.active_177b;put_half(state+26,(uint16_t)fade.fade.progress_178a);
 memcpy(state+32,registry.records,2560);for(unsigned i=0;i<3;i++){put_word(state+2592+i*4,(uint32_t)fade.motion.position[i]);put_word(state+2604+i*4,(uint32_t)fade.motion.velocity[i]);}memcpy(state+4152,&banks,1024);put_half(state+5176,mosaic);free(rom);
 if(_setmode(_fileno(stdout),_O_BINARY)==-1)return 7;return fwrite(state,1,sizeof state,stdout)==sizeof state?0:8;
}
