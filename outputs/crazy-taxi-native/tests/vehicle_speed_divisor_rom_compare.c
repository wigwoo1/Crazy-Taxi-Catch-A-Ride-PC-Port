#include "vehicle_config.h"
#include <stdio.h>
int main(void){int initial,ratio,final,rpm,a,b;unsigned flags;while(scanf("%d%u%d%d%d%d%d",&initial,&flags,&ratio,&final,&rpm,&a,&b)==7){int32_t ratios[7]={ratio};int32_t out=123;if(taxi_vehicle_speed_divisor(initial,(uint8_t)flags,1,ratios,final,(int16_t)rpm,a,b,&out))return 2;printf("%d\n",out);}return 0;}
