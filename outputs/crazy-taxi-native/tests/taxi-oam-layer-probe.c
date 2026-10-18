#include "taxi_oam_layer.h"
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
int main(void){if(_setmode(_fileno(stdin),_O_BINARY)<0||_setmode(_fileno(stdout),_O_BINARY)<0)return 5;TaxiTitleAssets assets={0};TaxiPaletteBanks palette={0};uint16_t oam[128][4];
 if(fread(oam,1,sizeof oam,stdin)!=sizeof oam||fread(assets.objects,1,sizeof assets.objects,stdin)!=sizeof assets.objects||fread(palette.objects,1,sizeof palette.objects,stdin)!=sizeof palette.objects)return 1;
 uint8_t *rgba=malloc(240*160*4);if(!rgba)return 2;
 int error=taxi_oam_layer(rgba,240,160,oam,&assets,&palette,0x06014000);
 if(error){free(rgba);return 3;}int ok=fwrite(rgba,1,240*160*4,stdout)==240*160*4;free(rgba);return ok?0:4;}
