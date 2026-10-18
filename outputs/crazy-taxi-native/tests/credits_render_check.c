#include "credits_asset.h"
#include <stdio.h>
#include <stdlib.h>
int main(int argc,char **argv){
 if(argc!=3)return 1;TaxiCreditsAsset a;if(taxi_credits_asset_load(&a,"outputs/crazy-taxi-native/assets/credits"))return 2;
 unsigned ticks=(unsigned)strtoul(argv[1],0,10);if(ticks>2016)return 3;
 FILE *f=fopen("outputs/crazy-taxi-native/assets/driving.ctr","rb");if(!f)return 4;
 uint8_t table[8196];if(fread(table,1,sizeof table,f)!=sizeof table||table[0]!='C'||table[1]!='T'||table[2]!='R'||table[3]!='1')return 5;fclose(f);
 TaxiCreditsFlow s;taxi_credits_flow_begin(&s);uint16_t scroll[3]={0,0,256};
 for(unsigned i=0;i<ticks;i++){
  unsigned row=s.row;uint16_t raw=(uint16_t)(table[4+2*s.angle]|(unsigned)table[5+2*s.angle]<<8);
  int32_t sine=raw<32768?raw:(int32_t)raw-65536;TaxiCreditsFrame q;
  if(taxi_credits_flow_step(&s,1,sine,s.line<104?a.lines[s.line]:0,1,0,0,&q)||taxi_credits_row_write(&a,row,&q))return 6;
  for(unsigned j=0;j<3;j++)scroll[j]=q.scroll[j];
 }
 uint16_t image[240*160];if(taxi_credits_compose(&a,scroll,image))return 7;
 f=fopen(argv[2],"wb");if(!f)return 8;
 for(unsigned i=0;i<240*160;i++){uint8_t b[2]={(uint8_t)image[i],(uint8_t)(image[i]>>8)};if(fwrite(b,1,2,f)!=2)return 9;}
 if(fclose(f))return 10;
 printf("PASS native credits diagnostic frame%u line%u row%u scroll%u/%u/%u; source assets, full fade/setup pixel oracle unverified.\n",ticks,s.line,s.row,scroll[0],scroll[1],scroll[2]);return 0;
}
