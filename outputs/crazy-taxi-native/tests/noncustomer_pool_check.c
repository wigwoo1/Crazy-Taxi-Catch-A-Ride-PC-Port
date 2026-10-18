#include "noncustomer_pool.h"
#include "session_init.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void put(uint8_t *p,uint32_t u){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(u>>(i*8));}
int main(int argc,char **argv){
 assert(argc==2);TaxiWorldAsset world={0};assert(!taxi_world_asset_load(&world,argv[1]));uint8_t raw[16];for(unsigned i=0;i<4;i++)put(raw+4*i,0x08010000+i*4);uint32_t ids[4]={0};assert(!taxi_noncustomer_bitmaps_decode(ids,raw,16));uint32_t old[4];memcpy(old,ids,16);
 assert(taxi_noncustomer_bitmaps_decode(ids,raw,15)==-1&&!memcmp(ids,old,16));memset(raw+4,0,4);assert(taxi_noncustomer_bitmaps_decode(ids,raw,16)==-1&&!memcmp(ids,old,16));
 TaxiNoncustomerState actors[64],before[64];memset(actors,0xa5,sizeof actors);memcpy(before,actors,sizeof before);
 TaxiSessionInit session;memset(&session,0xa5,sizeof session);session.reset_430c=64;
 TaxiNoncustomerPool pool={actors,64,&session.reset_430c,&world,ids};TaxiNoncustomerPool full={.count_430c=&session.reset_430c};
 assert(!taxi_noncustomer_pool_spawn(&full,NULL)&&session.reset_430c==64&&!memcmp(actors,before,sizeof actors));
 assert(!taxi_session_init(&session,240)&&!session.reset_430c&&!memcmp(actors,before,sizeof actors));
 uint8_t source[32]={0};source[3]=10;put(source+4,3350);put(source+12,128);
 session.reset_430c=0x8000;assert(taxi_noncustomer_pool_spawn(&pool,source)==-1&&!memcmp(actors,before,sizeof actors)&&session.reset_430c==0x8000);
 session.reset_430c=0;source[3]=14;assert(taxi_noncustomer_pool_spawn(&pool,source)==-1&&!memcmp(actors,before,sizeof actors));source[3]=10;
 pool.capacity=63;assert(taxi_noncustomer_pool_spawn(&pool,source)==-1&&!memcmp(actors,before,sizeof actors));pool.capacity=64;
 TaxiWorldAsset invalid={0};pool.world=&invalid;assert(taxi_noncustomer_pool_spawn(&pool,source)==-1&&!session.reset_430c);
 before[0].bytes[0]=0;before[0].bytes[1]=0;memset(before[0].bytes+2,0,6);before[0].bytes[48]=0;memcpy(before[0].bytes+8,source+4,4);memcpy(before[0].bytes+16,source+12,4);assert(!memcmp(actors,before,sizeof actors));
 pool.world=&world;put(actors[0].bytes+12,(uint32_t)-128);assert(!taxi_noncustomer_pool_passenger(&pool,127,source)&&session.reset_430c==1);assert(actors[0].bytes[0]==0&&actors[0].bytes[52]==64&&actors[0].bytes[53]==64);
 assert(!memcmp(actors+1,before+1,sizeof actors-sizeof actors[0]));
 assert(!taxi_session_init(&session,240)&&!session.reset_430c&&actors[0].bytes[52]==64);source[3]=13;assert(!taxi_noncustomer_pool_spawn(&pool,source)&&session.reset_430c==1&&actors[0].bytes[0]==3&&actors[0].bytes[53]==128&&actors[0].bytes[55]==7);
 taxi_world_asset_free(&world);puts("PASS native noncustomer pool capacity/full no-op, real B6EC count alias, retained slots, bitmap decode, invalid domain and ground-failure partial writes");return 0;
}
