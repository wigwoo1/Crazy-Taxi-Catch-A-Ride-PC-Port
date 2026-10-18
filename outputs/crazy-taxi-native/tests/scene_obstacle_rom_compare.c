#include "world_asset.h"
#include <stdio.h>
int main(int argc,char **argv){
 if(argc!=2)return 2;TaxiWorldAsset a={0};if(taxi_world_asset_load(&a,argv[1]))return 3;
 int x,y,z;while(scanf("%d%d%d",&x,&y,&z)==3){TaxiObstacleHit h;
  int rc=taxi_world_asset_obstacle(&a,(TaxiPoint){x,y,z},&h);if(rc<0)return 4;
  TaxiGroundHit g;int grc=taxi_world_asset_ground(&a,(TaxiPoint){x,y,z},&g);if(grc<0)return 5;
  printf("%d %d %u %d %d %d %d %d %u\n",rc,h.height,rc==2?h.polygon:65535,
      grc,g.height,g.normal.x,g.normal.y,g.normal.z,g.material);
 }taxi_world_asset_free(&a);return 0;
}
