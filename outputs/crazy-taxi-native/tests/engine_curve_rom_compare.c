#include "../src/engine_curve.h"
#include <stdio.h>
int main(void){int rpm,gear,up,scale;int32_t curve[16];for(int i=0;i<16;i++)curve[i]=20+i*100;while(scanf("%d%d%d%d",&rpm,&gear,&up,&scale)==4){uint32_t value=0;int rc=taxi_engine_curve_sample(curve,(int16_t)rpm,(int8_t)gear,(int16_t)up,scale,&value);printf("%d %u\n",rc,value);}return 0;}
