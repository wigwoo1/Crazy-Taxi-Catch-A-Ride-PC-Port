#include "source_display_native.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
_Static_assert(sizeof(TaxiSourceDisplayState)==40,"display fixture layout");
int main(void){TaxiSourceDisplayState state;TaxiTitleAssets assets={0};TaxiPaletteBanks palette;uint16_t oam[128][4];
 static uint8_t indices[1440],bitmap[38400],out[153600],work[153600];
 if(_setmode(_fileno(stdin),_O_BINARY)<0||_setmode(_fileno(stdout),_O_BINARY)<0)return 1;
 if(fread(&state,1,sizeof state,stdin)!=sizeof state||fread(bitmap,1,sizeof bitmap,stdin)!=sizeof bitmap||fread(oam,1,sizeof oam,stdin)!=sizeof oam||fread(assets.objects,1,sizeof assets.objects,stdin)!=sizeof assets.objects||fread(&palette,1,sizeof palette,stdin)!=sizeof palette)return 2;
 if(fread(indices,1,sizeof indices,stdin)!=sizeof indices)return 2;
 TaxiFlagHudPlan plan={.indicator_y=137,.indicator_variant=1};
 if(taxi_source_display_native(out,work,bitmap,&state,oam,&assets,&palette,&plan,indices,sizeof indices))return 3;
 return fwrite(out,1,sizeof out,stdout)==sizeof out?0:4;
}
