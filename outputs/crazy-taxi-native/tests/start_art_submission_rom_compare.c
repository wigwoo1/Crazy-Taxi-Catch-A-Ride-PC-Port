#include "start_art_submission.h"
#include <stdio.h>
int main(void){unsigned active,width;int phase,offset,scale;while(scanf("%u%d%u%d%d",&active,&phase,&width,&offset,&scale)==5){TaxiStartArtSubmission p;if(taxi_start_art_submission(&p,(uint8_t)active,(int16_t)phase,(uint16_t)width,(int16_t)offset,(int16_t)scale))return 2;printf("%u",p.count);for(unsigned j=0;j<p.count;j++)printf(" %u %u %ld %ld %d",p.frame[j],p.affine,(long)p.x[j],(long)p.y,p.affine?p.scale:0);puts("");}return 0;}
