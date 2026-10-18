/* Diagnostic supplied-Y endpoint query, not the original initializer. */
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "../src/world_asset.h"
#include "../src/destination.h"
int main(int argc,char **argv) {
    TaxiWorldAsset world={0};
    if(argc!=2 || taxi_world_asset_load(&world,argv[1])) return 1;
    TaxiPoint p; unsigned a,b,c,d;
    while(scanf("%" SCNd32 " %" SCNd32 " %" SCNd32 " %u %u %u %u",
                 &p.x,&p.y,&p.z,&a,&b,&c,&d)==7) {
        unsigned slots[4]={a,b,c,d};uint8_t record[48]={0};
        for(unsigned i=0;i<4;i++) {
            if(slots[i]>65535) {taxi_world_asset_free(&world);return 2;}
            record[40+2*i]=(uint8_t)slots[i];record[41+2*i]=(uint8_t)(slots[i]>>8);
        }
        TaxiGroundHit hit={0};
        int result=taxi_world_asset_ground(&world,p,&hit);
        if(result<0) {taxi_world_asset_free(&world);return 3;}
        printf("%d %u %" PRId32 " %d\n",result,result?hit.material:65535,
               result?hit.height:INT32_MIN,result?taxi_destination_accepts(0,record,hit.material):0);
    }
    taxi_world_asset_free(&world);
    return ferror(stdin)?4:0;
}

