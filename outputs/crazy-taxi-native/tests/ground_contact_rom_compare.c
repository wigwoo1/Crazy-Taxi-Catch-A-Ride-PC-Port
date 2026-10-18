#include "../src/world_asset.h"
#include <stdio.h>
int main(void){TaxiWorldAsset a={0};if(taxi_world_asset_load(&a,"outputs/crazy-taxi-native/assets/challenge-worlds/challenge5.ctw"))return 1;int x,y,z;while(scanf("%d%d%d",&x,&y,&z)==3){TaxiGroundHit h;int rc=taxi_world_asset_ground(&a,(TaxiPoint){x,y,z},&h);printf("%d %d %d %d %d %u %d %u\n",rc,h.height,h.normal.x,h.normal.y,h.normal.z,h.material,h.contact_valid,h.contact_value);}taxi_world_asset_free(&a);return 0;}
