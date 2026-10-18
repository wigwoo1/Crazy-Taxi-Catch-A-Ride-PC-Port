#include "traffic_init.h"
#include <limits.h>
unsigned taxi_traffic_density(uint8_t setting){
    static const uint8_t bytes[16]={5,11,17,22,27,33,39,44,50,28,26,24,22,20,18,16};
    return bytes[setting>>4];
}
uint8_t taxi_traffic_spacing(uint8_t setting){
    static const uint8_t bytes[16]={28,26,24,22,20,18,16,14,12,0,0,70,71,76,0,0};
    /* Original6C82 LSL24 then6C84 LSR28 selects the upper nibble. */
    return bytes[setting>>4];
}
int taxi_traffic_actors_reinitialize(TaxiTrafficActor *actors,size_t capacity,
    unsigned density,uint32_t parameters[3]){
    static const uint32_t defaults[3]={0x8e800,0xa0600,0x6b8cc};
    size_t count=density?density:1;
    if(!actors||!parameters||count>capacity||count>(unsigned)INT32_MAX/180u)return -1;
    for(size_t i=0;i<count;i++)actors[i].countdown=(int32_t)(180u*(i+1));
    for(unsigned i=0;i<3;i++)if(!parameters[i])parameters[i]=defaults[i];
    return 0;
}
static void put32(uint8_t *p, uint32_t value) {
    for (unsigned i = 0; i < 4; ++i) p[i] = (uint8_t)(value >> (8 * i));
}
static void sprites_initialize(uint8_t *sprites){
    static const uint32_t graphics[3]={0x0822ff08,0x08250110,0x08270318};
    for (unsigned type = 0; type < 3; ++type) {
        for (unsigned frame = 0; frame < 16; ++frame) {
            uint8_t *record = sprites + type * 192 + frame * 12;
            record[0] = 128; record[1] = 64; record[2] = 7; record[3] = 6;
            put32(record + 4, graphics[type] + 0x208 + frame * 0x2000);
        }
    }
}
int taxi_traffic_scene_reinitialize(TaxiTrafficActor *actors,size_t capacity,
    unsigned density,uint32_t parameters[3],uint8_t sprites[TAXI_TRAFFIC_SPRITE_BYTES]){
    if(!sprites||!actors||!parameters)return -1;
    if(taxi_traffic_actors_reinitialize(actors,capacity,density,parameters))return -1;
    sprites_initialize(sprites);return 0;
}
int taxi_traffic_initialize(uint8_t *actors, size_t actor_bytes, unsigned density,
                            uint32_t parameters[3], uint8_t *sprites) {
    static const uint32_t defaults[3] = {0x8e800, 0xa0600, 0x6b8cc};
    /* Source always writes actor zero, even if the table entry is zero. */
    size_t count = density ? density : 1;
    if (!actors || !parameters || !sprites ||
        count > actor_bytes / TAXI_TRAFFIC_RECORD_BYTES) return -1;
    for (size_t i = 0; i < count; ++i)
        put32(actors + i * TAXI_TRAFFIC_RECORD_BYTES + 0x10,
              (uint32_t)(180 * (i + 1)));
    for(unsigned type=0;type<3;type++)if(!parameters[type])parameters[type]=defaults[type];
    sprites_initialize(sprites);
    return 0;
}
