#include "vehicle_start_driver.h"
#include <string.h>
static uint32_t get(const uint8_t *p,unsigned n){uint32_t v=0;for(unsigned i=0;i<n;i++)v|=(uint32_t)p[i]<<(8*i);return v;}
static void put(uint8_t *p,uint32_t v,unsigned n){for(unsigned i=0;i<n;i++)p[i]=(uint8_t)(v>>(8*i));}
static int32_t value(const uint8_t *p,unsigned n){uint32_t v=get(p,n);int32_t s;memcpy(&s,&v,4);return s;}
#define POWER_FIELDS(X)  X(power.gears.flags_4ac,0x4ac,1) X(power.gears.flags_28,0x28,1) X(power.gears.max_gear,0x4b4,4) X(power.gears.reverse_ratio,0x4d4,4) X(power.gears.final_ratio,0x4d8,4)  X(power.gears.gear,0x4dc,1) X(power.gears.next_gear,0x4dd,1) X(power.gears.shift_timer,0x4de,2) X(power.gears.min_530,0x530,2) X(power.gears.max_532,0x532,2) X(power.gears.up_534,0x534,2) X(power.gears.down_536,0x536,2) X(power.gears.rpm_538,0x538,2)  X(power.gears.speed_544,0x544,4) X(power.gears.speed_568,0x568,4) X(power.gears.scale_5b8,0x5b8,4) X(power.gears.scale_5cc,0x5cc,4) X(power.gears.boost_75c,0x75c,4) X(power.gears.steering_578,0x578,2)  X(power.gears.state_e68,0xe68,1) X(power.gears.state_e69,0xe69,1) X(power.gears.phase_e6a,0xe6a,1) X(power.gears.phase_e6b,0xe6b,1) X(power.gears.timer_e6c,0xe6c,4) X(power.gears.timer_e70,0xe70,4) X(power.gears.timer_e78,0xe78,4) X(power.gears.timer_e7c,0xe7c,4) X(power.gears.timer_e80,0xe80,4)  X(power.efficiency_4e4,0x4e4,4) X(power.drag_528,0x528,4) X(power.rolling_52c,0x52c,4) X(power.force_540,0x540,4) X(power.boost_558,0x558,4) X(power.drag_590,0x590,4) X(power.mass_5a4,0x5a4,4) X(power.radius_5bc,0x5bc,4) X(power.radius_5d0,0x5d0,4) X(power.timer_e74,0xe74,4)  X(power.acceleration_6a0,0x6a0,4) X(power.response_53c,0x53c,4) X(power.maximum_550,0x550,4) X(power.state_6aa,0x6aa,1) X(power.scale_4b0,0x4b0,4) X(power.coasting_4e0,0x4e0,4) X(power.scale_e84,0xe84,4) X(power.brake_608,0x608,4) X(power.brake_60c,0x60c,4) X(power.state_6a9,0x6a9,1) X(steering_rate_57a,0x57a,2)
#define MOTION_FIELDS(X)  X(motion.motion.orientation.position.x,0,4) X(motion.motion.orientation.position.y,4,4) X(motion.motion.orientation.position.z,8,4) X(motion.motion.orientation.angle_18,0x18,2) X(motion.motion.orientation.angle_1a,0x1a,2) X(motion.motion.orientation.heading_1c,0x1c,4) X(motion.motion.orientation.heading_20,0x20,4)  X(motion.motion.orientation.drift_69c,0x69c,4) X(motion.motion.orientation.force_540,0x540,4) X(motion.motion.orientation.state_564,0x564,4) X(motion.motion.orientation.speed_544,0x544,4) X(motion.motion.orientation.timer_2a,0x2a,2) X(motion.motion.orientation.flags_358,0x358,1) X(motion.motion.orientation.flags_28,0x28,1) X(motion.motion.orientation.phase_e6a,0xe6a,1)  X(motion.motion.steering.speed_544,0x544,4) X(motion.motion.steering.state_564,0x564,4) X(motion.motion.steering.limit_7b0,0x7b0,4) X(motion.motion.steering.state_7b8,0x7b8,4) X(motion.motion.steering.angle_578,0x578,2) X(motion.motion.steering.flags_28,0x28,1) X(motion.motion.steering.flags_358,0x358,1) X(motion.motion.steering.phase_e6a,0xe6a,1) X(motion.motion.steering.phase_e6b,0xe6b,1)  X(motion.motion.scale_7c8,0x7c8,4) X(motion.motion.scale_e84,0xe84,4) X(motion.motion.dimension_594,0x594,4) X(motion.motion.response_57c,0x57c,2) X(motion.motion.displacement.x,0xc,4) X(motion.motion.displacement.y,0x10,4) X(motion.motion.displacement.z,0x14,4)  X(motion.force_53c,0x53c,4) X(motion.capacity_570,0x570,4) X(motion.capacity_574,0x574,4) X(motion.response_744,0x744,4) X(motion.flags_4ac,0x4ac,1)
int taxi_vehicle_start_driver_import(TaxiVehicleStartPacked *p,const TaxiDriver *d){if(!p||!d)return -1;
 #define IMPORT(f,o,n) put(p->actor+(o),(uint32_t)d->f,n);
 POWER_FIELDS(IMPORT)
 #undef IMPORT
 put(p->actor+0x53a,(uint16_t)d->rpm_53a,2);
 for(unsigned i=0;i<7;i++)put(p->actor+0x4b8+i*4,(uint32_t)d->power.gears.ratios[i],4);
 for(unsigned i=0;i<16;i++)put(p->actor+0x4e8+i*4,(uint32_t)d->power.curve[i],4);return 0;
}
int taxi_vehicle_start_driver_export(TaxiDriver *d,TaxiDriverContext *context,const TaxiVehicleStartPacked *p,uint8_t settings){if(!p||!d||!context)return -1;
 #define EXPORT(f,o,n) d->f=value(p->actor+(o),n);
 POWER_FIELDS(EXPORT) MOTION_FIELDS(EXPORT)
 #undef EXPORT
 d->rpm_53a=(int16_t)value(p->actor+0x53a,2);
 for(unsigned i=0;i<7;i++)d->power.gears.ratios[i]=value(p->actor+0x4b8+i*4,4);
 for(unsigned i=0;i<16;i++)d->power.curve[i]=value(p->actor+0x4e8+i*4,4);
 context->gear.state_2630=context->power.state_2630=context->motion.traction.state_2630=p->state_2630;
 context->motion.orientation.settings=settings;return 0;
}
