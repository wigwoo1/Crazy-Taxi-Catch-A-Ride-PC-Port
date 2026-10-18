#include "../src/world_polygon_stamp.h"
#include <stdio.h>
int main(void){unsigned seed,generation,reference,prior;while(scanf("%u %u %u %u",&seed,&generation,&reference,&prior)==4){uint32_t cache[16];for(unsigned j=0;j<16;j++)cache[j]=seed+j*0x1020304u;cache[reference]=prior;int rc=taxi_world_polygon_stamp(cache,16,generation,reference);printf("%d ",rc);for(unsigned j=0;j<16;j++)printf("%u ",cache[j]);puts("");}return 0;}
