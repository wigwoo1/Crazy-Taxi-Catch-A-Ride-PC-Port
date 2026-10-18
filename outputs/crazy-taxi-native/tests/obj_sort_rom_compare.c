#include "../src/obj_order.h"
#include <stdio.h>
int main(void){unsigned n;while(scanf("%u",&n)==1){if(n>128)return 2;TaxiObjRequest a[128]={0};for(unsigned i=0;i<n;i++){int x,z;if(scanf("%d %d",&x,&z)!=2)return 3;a[i].anchor[0]=(int16_t)x;a[i].anchor[1]=(int16_t)i;a[i].anchor[2]=(int16_t)z;a[i].anchor[3]=(int16_t)(100+i);for(unsigned j=0;j<4;j++)a[i].attributes[j]=(uint16_t)(i*4+j+1000);}if(taxi_obj_sort(a,n))return 4;for(unsigned i=0;i<n;i++){for(unsigned j=0;j<4;j++)printf("%d ",a[i].anchor[j]);for(unsigned j=0;j<4;j++)printf("%u ",a[i].attributes[j]);}puts("");}return 0;}
