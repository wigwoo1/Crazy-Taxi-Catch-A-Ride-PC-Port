#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "world_asset.h"
#include "passenger_asset.h"
#include "passenger_init.h"
static int32_t read32(const uint8_t *p){uint32_t u=p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);int32_t s;memcpy(&s,&u,4);return s;}
int main(void){
 TaxiWorldAsset world={0};TaxiPassengerAsset actors={0};TaxiPassengerState flag;
 assert(!taxi_world_asset_load(&world,"outputs/crazy-taxi-native/assets/challenge-worlds/challenge1.ctw"));
 assert(!taxi_passenger_asset_load(&actors,"outputs/crazy-taxi-native/assets/challenge-worlds/challenge1-actors.ctp"));assert(actors.count==1);
 const uint8_t *source=actors.source[0];TaxiPoint point={read32(source+4),read32(source+8),read32(source+12)};
 assert(point.x==87770&&point.y==707&&point.z==-3&&source[3]==0);
 int32_t heights[3];int32_t ys[3]={point.y,0,INT_MIN};
 for(unsigned i=0;i<3;i++){point.y=ys[i];assert(taxi_world_asset_height(&world,point,heights+i)==1);}
 memset(&flag,0,sizeof flag);
 TaxiPassengerResolved resolved={.kind=source[3],.origin={point.x,point.z},.spawn_height=heights[0]};
 assert(!taxi_passenger_init_flag(&flag,&resolved));assert(read32(flag.bytes+16)==87770&&read32(flag.bytes+24)==-3&&read32(flag.bytes+20)==heights[0]);
 printf("PASS native Crazy Flag ROM actor/world initialization: X87770 Z-3, ground Y%d (sourceY707), zeroY query%d, first-hit query%d; original retained actor Y ownership unverified.\n",heights[0],heights[1],heights[2]);
 taxi_passenger_asset_free(&actors);taxi_world_asset_free(&world);return 0;
}
