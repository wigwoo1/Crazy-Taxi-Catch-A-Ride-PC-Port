#include "spawn_placement.h"
#include <string.h>
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
static int32_t word(const uint8_t *b) {return bits(b[0]|((uint32_t)b[1]<<8)|((uint32_t)b[2]<<16)|((uint32_t)b[3]<<24));}
int taxi_spawn_marker_decode(TaxiSpawnMarker *out,const uint8_t *b,size_t size) {
    if(!out||!b||size<16) return -1;
    *out=(TaxiSpawnMarker){.query={word(b+4),INT32_MIN,word(b+8)},.heading=(uint16_t)((b[12]|((uint32_t)b[13]<<8))&4095)};
    return 0;
}
int taxi_spawn_place(TaxiWorldState *out,int32_t *heading_654,
    const TaxiSpawnMarker *marker,uint8_t settings,TaxiGroundQuery query,void *context) {
    if(!out||!heading_654||!marker||!query||marker->heading>4095) return -1;
    TaxiWorldState v=*out;
    int32_t heading=(int32_t)marker->heading*4096;
    int32_t back=(settings&0xe0)==0x80?heading:(int32_t)((marker->heading+2048)&4095)*4096;
    v.heading_1c=v.heading_20=heading;v.angle_18=v.angle_1a=0;
    TaxiGroundHit hit={.height=INT32_MIN};TaxiWorldContact c={0};
    int result=query(context,marker->query,&hit);if(result<0||result>1) return -1;
    int placed=result&&taxi_world_contact(&c,&hit);
    if(placed) {
        v.height.position=(TaxiPoint){marker->query.x,bits((uint32_t)c.height+64),marker->query.z};
        v.ground_36c=c.height;v.red_374=c.red;v.green_375=c.green;v.blue_376=c.blue;
    }
    *out=v;*heading_654=back;return placed;
}
