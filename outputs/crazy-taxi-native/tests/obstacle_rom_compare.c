#include "../src/world_asset.h"
#include <stdio.h>
int main(void){TaxiWorldAsset a={0};if(taxi_world_asset_load(&a,"outputs/crazy-taxi-native/assets/challenge-worlds/challenge5.ctw"))return 1;int x,y,z;while(scanf("%d%d%d",&x,&y,&z)==3){TaxiObstacleHit h;int rc=taxi_world_asset_obstacle(&a,(TaxiPoint){x,y,z},&h);printf("%d %d %u\n",rc,h.height,h.polygon);}taxi_world_asset_free(&a);return 0;}
