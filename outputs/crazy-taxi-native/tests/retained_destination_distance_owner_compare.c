/* Offline captured-state adapter for production E034 scalar behavior. */
#include "../src/destination_guidance.h"
#include <inttypes.h>
#include <stdio.h>
int main(void){
 int32_t selected,x,z;unsigned present;uint8_t record[48];
 if(scanf("%" SCNd32 " %" SCNd32 " %" SCNd32 " %u",&selected,&x,&z,&present)!=4||present>1)return 2;
 for(unsigned i=0;i<48;i++){unsigned value;if(scanf("%u",&value)!=1||value>255)return 2;record[i]=(uint8_t)value;}
 printf("%" PRId32 "\n",taxi_destination_distance(selected,present?record:NULL,x,z));return 0;
}
