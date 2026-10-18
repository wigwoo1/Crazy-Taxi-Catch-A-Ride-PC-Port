#include "../src/world_asset.h"
int main(int argc,char **argv) {
    if(argc!=2) return 2;
    TaxiWorldAsset a={0};int result=taxi_world_asset_load(&a,argv[1]);taxi_world_asset_free(&a);return result?1:0;
}
