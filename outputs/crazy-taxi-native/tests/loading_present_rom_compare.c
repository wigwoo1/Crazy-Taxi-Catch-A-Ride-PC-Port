#include "loading_present.h"
#include <stdio.h>
static unsigned n;static uint32_t events[16][13];
static int call(void *c,TaxiLoadingPresent *s,uint32_t target,const uint32_t a[9],uint32_t pos[3],uint32_t *out){
 (void)c;(void)s;uint32_t *v=events[n++];v[0]=target;for(unsigned i=0;i<9;i++)v[i+1]=a[i];for(unsigned i=0;i<3;i++)v[i+10]=pos?pos[i]:0;
 *out=target==0x08028fe4?0x08400000+a[0]*16:0;return 0;
}
int main(void){unsigned a,b,w,h,animation;while(scanf("%u %u %u %u %u",&a,&b,&w,&h,&animation)==5){
 TaxiLoadingPresent s={{(uint16_t)w,(uint16_t)h,(uint8_t)a,(uint8_t)b},(uint8_t)animation};n=0;if(taxi_loading_present(&s,call,NULL))return 1;
 printf("%u",n);for(unsigned i=0;i<n;i++)for(unsigned j=0;j<13;j++)printf(" %u",events[i][j]);puts("");
 }return 0;}
