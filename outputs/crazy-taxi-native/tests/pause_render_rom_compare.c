#include "pause_render.h"
#include <stdio.h>
int main(void){unsigned frozen,selection,dialog,no,outer,fade,width;
 while(scanf("%u %u %u %u %u %u %u",&frozen,&selection,&dialog,&no,&outer,&fade,&width)==7){
  TaxiPauseMenu s={.frozen=(uint8_t)frozen,.selection=(uint8_t)selection,.dialog=(uint8_t)dialog,.no_selected=(uint8_t)no,.outer_state=(uint8_t)outer};TaxiPauseRenderRequest r[6];unsigned n;uint8_t palette=0;
  if(taxi_pause_render_requests(&s,(uint8_t)fade,(uint16_t)width,r,&n,&palette))return 2;
  printf("%u %u",palette,n);for(unsigned j=0;j<n;j++)printf(" %u %u %d %d %u",r[j].type,r[j].id,r[j].x,r[j].y,r[j].group_or_palette);puts("");
 }return 0;
}
