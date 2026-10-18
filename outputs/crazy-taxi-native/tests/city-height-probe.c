#include "../src/world_asset.h"
#include <stdio.h>
int main(int argc,char **argv){if(argc!=2)return 1;TaxiWorldAsset a={0};if(taxi_world_asset_load(&a,argv[1]))return 2;int x,y,z;while(scanf("%d%d%d",&x,&y,&z)==3){int32_t h;int rc=taxi_world_asset_height(&a,(TaxiPoint){x,y,z},&h);printf("%d %d\n",rc,h);}taxi_world_asset_free(&a);return 0;}
