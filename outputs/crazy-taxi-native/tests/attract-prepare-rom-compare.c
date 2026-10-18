#include "attract_prepare.h"
#include <stdio.h>
int main(void){unsigned seed;while(scanf("%u",&seed)==1){TaxiAttractPrepare s;for(unsigned i=0;i<20;i++)s.recorder[i]=(uint8_t)(seed+i*37);s.flag_1ea8=(uint8_t)(seed+91);s.flag_3895=(uint8_t)(seed+153);taxi_attract_prepare(&s);for(unsigned i=0;i<20;i++)printf("%02x",s.recorder[i]);printf("%02x%02x\n",s.flag_1ea8,s.flag_3895);}return 0;}
