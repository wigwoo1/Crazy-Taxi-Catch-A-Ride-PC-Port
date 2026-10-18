#include "sprite_copy_flush_owned.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
int main(int argc,char **argv){if(argc!=3)return 1;FILE *f=fopen(argv[1],"rb"),*r=fopen(argv[2],"rb");if(!f||!r)return 2;uint8_t state[33538];if(fread(state,1,sizeof state,f)!=sizeof state)return 3;fclose(f);uint8_t *rom=malloc(8388608);if(!rom||fread(rom,1,8388608,r)!=8388608)return 4;fclose(r);TaxiSpriteCopies copies={.count=(uint16_t)(state[0]|(unsigned)state[1]<<8)};memcpy(copies.records,state+2,768);TaxiSpriteImmediateCopy immediate={0};TaxiSpriteCopyFlushOwner owner={rom,8388608,state+770,32768,0x06010000,&copies,&immediate};if(taxi_sprite_copy_flush_owned(&owner))return 5;state[0]=(uint8_t)copies.count;state[1]=(uint8_t)(copies.count>>8);memcpy(state+2,copies.records,768);free(rom);if(_setmode(_fileno(stdout),_O_BINARY)==-1)return 6;return fwrite(state,1,sizeof state,stdout)==sizeof state?0:7;}
