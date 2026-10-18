#include "taxi_half_plan.h"
#include <stdio.h>
int main(void){int x,y,focal,distance,base,sine,cosine;while(scanf("%d%d%d%d%d%d%d",&x,&y,&focal,&distance,&base,&sine,&cosine)==7){TaxiHalfPlan plan;if(taxi_half_plan(&plan,x,y,focal,distance,base,sine,cosine))return 1;printf("%d %d %d %d %d\n",plan.scale,plan.centers[0][0],plan.centers[0][1],plan.centers[1][0],plan.centers[1][1]);}return 0;}
