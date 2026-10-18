#include "vehicle_start_world.h"
#include <string.h>
static uint32_t get(const uint8_t *p,unsigned n){uint32_t v=0;for(unsigned i=0;i<n;i++)v|=(uint32_t)p[i]<<(8*i);return v;}
static void put(uint8_t *p,uint32_t v,unsigned n){for(unsigned i=0;i<n;i++)p[i]=(uint8_t)(v>>(8*i));}
static int32_t word(const uint8_t *p){uint32_t v=get(p,4);int32_t s;memcpy(&s,&v,4);return s;}
static int16_t half(const uint8_t *p){uint16_t v=(uint16_t)get(p,2);int16_t s;memcpy(&s,&v,2);return s;}
#define FIELDS(X)  X(height.position.x,0,4) X(height.position.y,4,4) X(height.position.z,8,4)  X(displacement_x,0xc,4) X(height.displacement_y,0x10,4) X(displacement_z,0x14,4)  X(angle_18,0x18,2) X(angle_1a,0x1a,2) X(heading_1c,0x1c,4) X(heading_20,0x20,4)  X(height.flags_28,0x28,1) X(camera_flags_29,0x29,1) X(state_2c,0x2c,2) X(impulse_34,0x34,4) X(angle_38,0x38,2) X(angle_3a,0x3a,2) X(height_3c,0x3c,4) X(state_40,0x40,4) X(state_44,0x44,4)  X(ground_36c,0x36c,4) X(red_374,0x374,1) X(green_375,0x375,1) X(blue_376,0x376,1) X(contact_378,0x378,4) X(previous_37c,0x37c,4)  X(speed_544,0x544,4) X(height.state_698,0x698,4) X(drift_69c,0x69c,4)  X(height.body.orientation_6f4.x,0x6f4,4) X(height.body.orientation_6f4.y,0x6f8,4) X(height.body.orientation_6f4.z,0x6fc,4)  X(height.body.response_704,0x704,4) X(height.body.response_70c,0x70c,4)  X(height.body.position_734,0x734,4) X(height.body.velocity_73c,0x73c,4) X(height.body.position_740,0x740,4) X(height.body.velocity_748,0x748,4)  X(impulse_74c,0x74c,4) X(impulse_750,0x750,4) X(height.body.denominator_754,0x754,4) X(height.body.denominator_758,0x758,4)  X(material_e60,0xe60,2) X(height.scale_e84,0xe84,4)  X(snapshot_714.material,0x714,4) X(snapshot_714.height,0x718,4) X(snapshot_714.normal.x,0x71c,4) X(snapshot_714.normal.y,0x720,4) X(snapshot_714.normal.z,0x724,4) X(snapshot_714.flags,0x728,1) X(snapshot_714.red,0x729,1) X(snapshot_714.green,0x72a,1) X(snapshot_714.blue,0x72b,1) X(snapshot_714.contact_value,0x72c,4)
int taxi_vehicle_start_world_import(TaxiVehicleStartPacked *p,const TaxiWorldState *w){if(!p||!w)return -1;
 #define IMPORT(f,o,n) put(p->actor+(o),(uint32_t)w->f,n);
 FIELDS(IMPORT)
 #undef IMPORT
 put(p->actor+0x730,(uint16_t)w->height.body.velocity_730,2);put(p->actor+0x732,(uint16_t)w->height.body.velocity_732,2);return 0;
}
int taxi_vehicle_start_world_export(TaxiWorldState *w,const TaxiVehicleStartPacked *p){if(!p||!w)return -1;
 #define EXPORT(f,o,n) w->f=(n)==4?word(p->actor+(o)):get(p->actor+(o),n);
 FIELDS(EXPORT)
 #undef EXPORT
 w->height.body.velocity_730=half(p->actor+0x730);w->height.body.velocity_732=half(p->actor+0x732);return 0;
}
