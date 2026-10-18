#include "palette_fade_setup.h"
#include <stdio.h>
#include <inttypes.h>
int main(void){uint32_t mode,cap,bg,obj;while(scanf("%"SCNu32" %"SCNu32" %"SCNu32" %"SCNu32,&mode,&cap,&bg,&obj)==4){
 TaxiPaletteFadeSetup s={0};if(taxi_palette_fade_setup(&s,mode,cap,bg,obj))return 1;
 printf("%u %u %u %u %u",s.mode,s.cap,s.endpoint_count,s.background_mask,s.object_mask);
 for(unsigned i=0;i<3;i++)printf(" %u",s.background_channels[i]);
 for(unsigned i=0;i<3;i++)printf(" %u",s.object_channels[i]);puts("");
 }return 0;}
