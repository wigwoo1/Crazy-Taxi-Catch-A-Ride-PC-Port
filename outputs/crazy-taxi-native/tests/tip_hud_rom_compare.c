#include "../src/tip_hud.h"
#include <stdio.h>
int main(void){unsigned frame,pause,v;uint8_t tips[10][56];TaxiTipHudRequest q[10];while(scanf("%u %u",&frame,&pause)==2){for(unsigned i=0;i<10;i++)for(unsigned j=0;j<56;j++){if(scanf("%u",&v)!=1)return 1;tips[i][j]=(uint8_t)v;}int n=taxi_tip_hud_requests(tips,(uint16_t)frame,(uint8_t)pause,q);if(n<0)return 1;printf("%d",n);for(int i=0;i<n;i++)printf(" %ld %ld %u %ld %ld %u %s",(long)q[i].image_x,(long)q[i].image_y,q[i].image_address,(long)q[i].text_x,(long)q[i].text_y,q[i].color,q[i].text);puts("");}return 0;}
