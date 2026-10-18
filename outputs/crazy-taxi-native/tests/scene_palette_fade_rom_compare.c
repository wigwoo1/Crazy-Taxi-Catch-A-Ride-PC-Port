#include "palette_fade.h"
#include <stdio.h>
int main(void){
 unsigned source[256];for(unsigned k=0;k<256;k++)if(scanf("%u",source+k)!=1)return 2;
 TaxiPaletteFade fade={0};taxi_palette_fade_begin(&fade,1);
 for(unsigned tick=0;tick<35;tick++){
  int cap=fade.progress_178a;taxi_palette_fade_step(&fade,1);
  printf("%d %d %u",cap,fade.progress_178a,fade.active_177b);
  for(unsigned k=0;k<256;k++)printf(" %u",taxi_palette_fade_cap_rgb555((uint16_t)source[k],(int16_t)cap));puts("");
 }return 0;
}
