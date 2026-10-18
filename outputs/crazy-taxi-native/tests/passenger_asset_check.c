#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/passenger_asset.h"
int main(int argc,char **argv) {
    if(argc!=2&&argc!=3) return 2;
    TaxiPassengerAsset a={0};int status=taxi_passenger_asset_load(&a,argv[1]);
    if(argc==3) {assert(status==-1&&!a.source&&!a.routes&&!a.destinations&&!a.count);return 0;}
    assert(!status);
    printf("%u %u %u\n",a.count,a.route_count,a.destination_count);
    if(a.count) {
        uint8_t (*before)[32]=a.source;
        assert(taxi_passenger_asset_load(&a,argv[1])==-1&&a.source==before);
    }
    taxi_passenger_asset_free(&a);taxi_passenger_asset_free(&a);return 0;
}
