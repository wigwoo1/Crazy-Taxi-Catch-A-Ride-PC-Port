#include "palette_fade.h"
#include <stdio.h>
int main(void){unsigned mode,color;while(scanf("%u %u",&mode,&color)==2){if(mode>1||color>65535)return 2;printf("%u\n",mode?taxi_palette_gameplay_alternate_rgb555((uint16_t)color):(uint16_t)color);}return 0;}
