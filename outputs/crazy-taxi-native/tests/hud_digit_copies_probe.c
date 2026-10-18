#include "../src/hud_digit_copies.h"
#include <stdio.h>
#include <assert.h>
int main(void){int v[19];while(scanf("%d",v)==1){for(unsigned i=1;i<19;i++)assert(scanf("%d",v+i)==1);int16_t a[10],b[9];for(unsigned i=0;i<10;i++)a[i]=(int16_t)v[i];for(unsigned i=0;i<9;i++)b[i]=(int16_t)v[i+10];TaxiHudDigitCopyRequest requests[15];int n=taxi_hud_digit_copy_requests(requests,a,b);assert(n==15);for(unsigned i=0;i<15;i++)printf("%u %u %u %u %u%s",requests[i].source,requests[i].index,requests[i].frame,requests[i].group,requests[i].immediate,i==14?"\n":" ");}return 0;}
