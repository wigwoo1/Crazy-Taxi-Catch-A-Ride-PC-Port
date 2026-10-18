#include "../src/world_render.h"
#include <stdio.h>
int main(int argc,char **argv){if(argc!=3)return 2;TaxiWorldAsset worlds[2]={{0}};for(unsigned i=0;i<2;i++)if(taxi_world_asset_load(worlds+i,argv[i+1]))return 3;unsigned city,ref;int x,y,z;while(scanf("%u %u %d %d %d",&city,&ref,&x,&y,&z)==5){if(city>1)return 4;printf("%d\n",taxi_world_polygon_facing(worlds+city,ref,(TaxiPoint){x,y,z}));}for(unsigned i=0;i<2;i++)taxi_world_asset_free(worlds+i);return 0;}
