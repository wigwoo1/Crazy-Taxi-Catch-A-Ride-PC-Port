#include "shade_table.h"
#include <stdio.h>
int main(void){unsigned red,green,blue,color;
 while(scanf("%u%u%u",&red,&green,&blue)==3){
  uint16_t palette[256];for(unsigned i=0;i<256;i++){if(scanf("%u",&color)!=1)return 2;palette[i]=(uint16_t)color;}
  uint32_t packed=0;if(!taxi_palette_nearest_pair(palette,red,green,blue,&packed))return 3;
  printf("%u\n",packed);
 }return 0;
}
