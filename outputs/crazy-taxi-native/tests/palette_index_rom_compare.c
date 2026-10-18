#include "shade_table.h"
#include <stdio.h>
int main(void){unsigned r,g,b,p;while(scanf("%u%u%u",&r,&g,&b)==3){uint16_t palette[256];for(unsigned i=0;i<256;i++){if(scanf("%u",&p)!=1)return 2;palette[i]=(uint16_t)p;}uint8_t index=255;if(!taxi_palette_nearest_index(palette,r,g,b,&index))return 3;printf("%u\n",index);}return 0;}
