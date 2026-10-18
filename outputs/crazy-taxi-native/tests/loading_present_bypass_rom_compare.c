#include "loading_present_bypass.h"
#include <stdio.h>
static unsigned calls;static uint32_t captured[5];
static int clear(void *unused,TaxiLoadingPresentGate *s,uint32_t target,const uint32_t args[4]){
 (void)unused;(void)s;++calls;captured[0]=target;for(unsigned i=0;i<4;++i)captured[i+1]=args[i];return 0;
}
int main(void){unsigned a,b,w,h;while(scanf("%u %u %u %u",&a,&b,&w,&h)==4){
 TaxiLoadingPresentGate s={(uint16_t)w,(uint16_t)h,(uint8_t)a,(uint8_t)b};calls=0;
 int result=taxi_loading_present_bypass(&s,clear,NULL);
 printf("%d %u",result,calls);for(unsigned i=0;i<5;++i)printf(" %u",captured[i]);puts("");
 }return 0;}
