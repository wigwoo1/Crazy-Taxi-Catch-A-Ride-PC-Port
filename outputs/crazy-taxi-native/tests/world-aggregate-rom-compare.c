#include "../src/world_render.h"
#include <stdio.h>
int main(void){unsigned count;while(scanf("%u",&count)==1){if(count!=3&&count!=4)return 2;TaxiPoint vertices[4]={{0}};unsigned flags[4];for(unsigned i=0;i<count;i++){int z;if(scanf("%u %d",flags+i,&z)!=2)return 3;vertices[i].z=z;}TaxiWorldRenderPolygon out={0};int rc=taxi_world_render_aggregate(vertices,flags,count,&out);printf("%d %ld %u %ld\n",rc,(long)out.depth,out.clip_flags,(long)out.max_depth);}return 0;}
