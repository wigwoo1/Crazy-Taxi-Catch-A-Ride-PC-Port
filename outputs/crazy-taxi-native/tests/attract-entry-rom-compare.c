#include "attract_entry.h"
#include <stdio.h>
#include "attract_prepare.h"
#include <string.h>
int main(int argc,char **argv){int prepared=argc==2&&!strcmp(argv[1],"--prepare");unsigned seed,fill;while(scanf("%u %u",&seed,&fill)==2){TaxiAttractEntry s={0};for(unsigned i=0;i<8;i++)s.settings[i]=(uint8_t)(fill+i*19);s.flag_153c=s.flag_1697=165;TaxiAttractPrepare prep;for(unsigned j=0;j<20;j++)prep.recorder[j]=(uint8_t)(fill+j*37);prep.flag_1ea8=91;prep.flag_3895=153;if(prepared)taxi_attract_prepare(&prep);uint32_t rng=seed;if(taxi_attract_entry(&s,prepared?prep.recorder[0]:1,&rng))return 1;for(unsigned i=0;i<8;i++)printf("%02x",s.settings[i]);printf(" %u %u %u %u %u\n",s.flag_153c,s.flag_1697,s.saved_city,s.scene_identity,rng);if(prepared){for(unsigned j=0;j<20;j++)printf("%02x",prep.recorder[j]);printf("%02x%02x\n",prep.flag_1ea8,prep.flag_3895);}}return 0;}
