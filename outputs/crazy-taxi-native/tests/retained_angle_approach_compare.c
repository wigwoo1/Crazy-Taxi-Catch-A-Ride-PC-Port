#include "../src/traffic_heading.h"
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
int main(void){uint32_t current,target;int32_t step;while(scanf("%" SCNu32 " %" SCNu32 " %" SCNd32,&current,&target,&step)==3)printf("%" PRIu32 "\n",taxi_traffic_heading_approach(current,target,step));return 0;}
