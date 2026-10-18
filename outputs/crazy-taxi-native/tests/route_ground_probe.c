#include "world_asset.h"
#include <stdio.h>
int main(int argc,char**argv){if(argc!=2)return 2;TaxiWorldAsset a={0};if(taxi_world_asset_load(&a,argv[1]))return 3;int x,y,z;while(scanf("%d%d%d",&x,&y,&z)==3){TaxiGroundHit h;int r=taxi_world_asset_ground(&a,(TaxiPoint){x,y,z},&h);printf("%d %d %d %d %d %u %u\n",r,h.height,h.normal.x,h.normal.y,h.normal.z,h.material,h.polygon);}taxi_world_asset_free(&a);return 0;}
