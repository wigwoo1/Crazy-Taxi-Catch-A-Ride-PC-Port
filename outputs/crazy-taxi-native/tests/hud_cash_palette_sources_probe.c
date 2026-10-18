#include "hud_cash_pixels.h"
#include <stdio.h>
int main(void){unsigned seed;while(scanf("%u",&seed)==1){TaxiPaletteRegistration r={0};r.count=11;for(unsigned i=0;i<128;i++)for(unsigned b=0;b<20;b++)r.records[i][b]=(uint8_t)(seed+i*31+b*17);if(taxi_hud_cash_palette_sources(&r))return 1;for(unsigned i=0;i<128;i++)for(unsigned b=0;b<20;b++)printf("%02x",r.records[i][b]);putchar('\n');}return 0;}
