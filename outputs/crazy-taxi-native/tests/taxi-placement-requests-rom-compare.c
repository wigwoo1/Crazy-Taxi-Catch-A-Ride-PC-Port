#include "taxi_placement_requests.h"
#include <stdio.h>
int main(void){unsigned scale,roll;while(scanf("%u%u",&scale,&roll)==2){const uint32_t left[3]={106,102,3537},right[3]={133,102,3537};TaxiPlacementRequest out[2];taxi_placement_requests(left,right,scale,(uint16_t)roll,out);for(unsigned i=0;i<2;i++){TaxiPlacementRequest *p=out+i;printf("%u %u %u %u %u %u %u %u %u %d %d%s",p->frame,p->group,p->position[0],p->position[1],p->position[2],p->priority,p->bucket,p->flags,p->palette,p->scale,p->angle,i?"\n":" ");}}return 0;}
