#include "driving_shared_snapshot.h"
#include <string.h>
static uint32_t word(const uint8_t *p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static int32_t signed_word(const uint8_t *p){uint32_t u=word(p);int32_t s;memcpy(&s,&u,4);return s;}
static uint16_t half(const uint8_t *p){return (uint16_t)(p[0]|((uint16_t)p[1]<<8));}
int taxi_driving_shared_snapshot_read(TaxiDrivingSharedSnapshot *out,const uint8_t *e,size_t en,const uint8_t *i,size_t in){
 if(!out||!e||!i||en<0x7603||in<48)return -1;
 TaxiDrivingSharedSnapshot s={0};TaxiDriverContext *c=&s.context;
 s.delta=signed_word(i);s.frame_delta=signed_word(e+0x3b00);memcpy(s.command.bytes,i+16,16);
 s.settings=e[0x1eb0];s.flag_1697=e[0x1697];s.flag_153c=e[0x153c];s.freeze_177b=e[0x177b];s.sound_enabled=e[0x7602];
 c->gear.state_2630=c->power.state_2630=c->motion.traction.state_2630=signed_word(e+0x2630);
 c->gear.state_1654=c->power.state_1654=c->motion.orientation.amount_1654=signed_word(e+0x1654);
 c->gear.state_25d4=c->motion.traction.flag_25d4=e[0x25d4];c->power.state_25d2=e[0x25d2];c->power.state_25dc=signed_word(e+0x25dc);c->freeze_2620=e[0x2620];
 c->motion.orientation.delta=s.delta;c->motion.orientation.settings=s.settings;c->motion.orientation.submode=e[0x1537];c->motion.orientation.flag_1697=s.flag_1697;c->motion.orientation.flag_1ea8=e[0x1ea8];c->motion.orientation.flag_177b=s.freeze_177b;c->motion.orientation.flag_1694=e[0x1694];
 c->motion.orientation.counter_164c=half(e+0x164c);c->motion.orientation.counter_1650=half(e+0x1650);c->motion.orientation.last_heading_1658=signed_word(e+0x1658);
 c->motion.traction.settings=s.settings;c->motion.traction.flag_1696=e[0x1696];c->motion.traction.flag_1697=s.flag_1697;c->motion.traction.flag_262c=e[0x262c];
 *out=s;return 0;
}
