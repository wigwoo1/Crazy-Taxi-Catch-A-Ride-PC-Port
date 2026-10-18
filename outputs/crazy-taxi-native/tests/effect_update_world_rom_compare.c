#include "../src/effect_update.h"
#include <stdio.h>
#include "../src/world_asset.h"
#include <string.h>
#include <limits.h>
static void put(unsigned char*p,unsigned u){for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(u>>(8*i));}
int main(void){TaxiWorldAsset world={0};if(taxi_world_asset_load(&world,"outputs/crazy-taxi-native/assets/challenge-worlds/challenge5.ctw"))return 2;unsigned tag,frame,size,clock;int delta,timer,x,y,z;while(scanf("%u%d%d%u%u%u%d%d%d",&tag,&delta,&timer,&frame,&size,&clock,&x,&y,&z)==9){TaxiEffectRecord r;memset(&r,0x5a,sizeof r);put(r.bytes,2);put(r.bytes+0x14,0xfffff000u);put(r.bytes+0x18,8192);put(r.bytes+0x1c,4095);put(r.bytes+0x24,3072);put(r.bytes+0x28,0xfffffc00u);put(r.bytes+0x2c,8192);put(r.bytes+0x40,(unsigned)timer);r.bytes[0x35]=(unsigned char)frame;r.bytes[0x36]=(unsigned char)size;r.bytes[0x37]=(unsigned char)(size>>8);int rc=taxi_effect_update_tag(&r,tag,(TaxiPoint){x,y,z},delta,clock,taxi_world_asset_height,&world);if(rc<0)return 1;for(unsigned i=0;i<72;i++)printf("%u%c",r.bytes[i],i==71?'\n':' ');}taxi_world_asset_free(&world);return 0;}
