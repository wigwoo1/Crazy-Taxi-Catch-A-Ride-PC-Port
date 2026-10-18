#include "simulation_start.h"
#include "vehicle_config.h"
#include "vehicle_reset.h"
#include "vehicle_start_finalize.h"
#include <string.h>
static int32_t word(const uint8_t *b,unsigned o) {
    b+=o-0x4ac;uint32_t u=b[0]|((uint32_t)b[1]<<8)|((uint32_t)b[2]<<16)|((uint32_t)b[3]<<24);
    int32_t s;memcpy(&s,&u,4);return s;
}
static int16_t half(const uint8_t *b,unsigned o) {
    b+=o-0x4ac;uint16_t u=(uint16_t)(b[0]|((unsigned)b[1]<<8));int16_t s;memcpy(&s,&u,2);return s;
}
static int8_t byte(const uint8_t *b,unsigned o) {int8_t s;memcpy(&s,b+o-0x4ac,1);return s;}
int taxi_simulation_start(TaxiDriveWorld *out,TaxiDriveWorldContext *context,
    const uint8_t *b,size_t size,const TaxiSpawnMarker *marker,uint8_t settings,
    TaxiGroundQuery ground,void *query_context) {
    if(!out||!context) return -1;
    TaxiVehicleConfig config;if(taxi_vehicle_config_decode(&config,b,size)) return -1;
    TaxiDriveWorld v={0};TaxiDriveWorldContext c={0};int32_t heading654;
    v.speed_divisor_548=config.speed_divisor_548;v.camera_offset_690=taxi_vehicle_start_rear_offset();
    int placed=taxi_spawn_place(&v.world,&heading654,marker,settings,ground,query_context);
    if(placed<0) return -1;
    TaxiAcceleration *p=&v.driver.power;TaxiGearState *g=&p->gears;
    g->flags_4ac=config.flags_4ac;g->max_gear=config.gear_count;
    memcpy(g->ratios,config.ratios,sizeof g->ratios);
    g->reverse_ratio=word(b,0x4d4);g->final_ratio=config.final_ratio;
    g->gear=0;g->next_gear=byte(b,0x4dd);g->shift_timer=half(b,0x4de);
    g->min_530=config.rpm_min;g->max_532=config.rpm_max;
    g->up_534=half(b,0x534);g->down_536=half(b,0x536);g->rpm_538=half(b,0x538);
    g->speed_544=word(b,0x544);g->speed_568=word(b,0x568);
    g->scale_5b8=config.scale_5b8;g->scale_5cc=config.scale_5cc;g->steering_578=half(b,0x578);
    p->efficiency_4e4=word(b,0x4e4);p->drag_528=config.limit_528;p->rolling_52c=word(b,0x52c);
    p->force_540=word(b,0x540);
    p->boost_558=word(b,0x558);p->drag_590=config.scale_590;p->mass_5a4=word(b,0x5a4);
    p->radius_5bc=config.scale_5bc;p->radius_5d0=config.scale_5d0;
    p->acceleration_6a0=word(b,0x6a0);p->response_53c=word(b,0x53c);p->maximum_550=0; /* 08014cc0. */
    p->state_6aa=b[0x6aa-0x4ac];p->state_6a9=b[0x6a9-0x4ac];
    memcpy(p->curve,config.curve,sizeof p->curve);p->scale_4b0=word(b,0x4b0);p->coasting_4e0=word(b,0x4e0);
    p->scale_e84=4096;p->brake_608=word(b,0x608);p->brake_60c=word(b,0x60c);
    v.driver.steering_rate_57a=half(b,0x57a);
    TaxiVehicleReset reset={.orientation={.position=v.world.height.position,
        .heading_1c=v.world.heading_1c,.heading_20=v.world.heading_20,
        .speed_544=g->speed_544,.force_540=p->force_540,.flags_358=4,.state_564=config.state_564}};
    taxi_vehicle_reset(&reset,settings,&c.driver.gear.state_2630);
    v.initial_camera_pitch_650=reset.state_650;
    v.initial_camera_heading_654=reset.state_654;
    v.initial_camera_mode_64c=reset.state_64c;
    g->boost_75c=reset.state_75c;
    v.driver.motion.motion.orientation=reset.orientation;
    /* Actor tail08014d6e sets358bit0;08014d76 sets28bit4/clearsbit3.
       The helper08011804 already sets358bit2 after its caller's earlier gate. */
    v.driver.motion.motion.orientation.flags_358|=1;
    g->flags_28=v.world.height.flags_28=v.driver.motion.motion.orientation.flags_28=16;
    v.driver.motion.motion.dimension_594=config.dimension_594;
    v.driver.motion.motion.scale_7c8=4096; /* 08014d30..36. */
    v.driver.motion.motion.scale_e84=v.world.height.scale_e84=4096;
    v.driver.motion.capacity_570=config.capacity_570;v.driver.motion.capacity_574=config.capacity_574;
    v.driver.motion.flags_4ac=g->flags_4ac;v.driver.motion.force_53c=p->response_53c;
    v.world.height.body.denominator_754=reset.state_754;v.world.height.body.denominator_758=reset.state_758;
    v.world.material_e60=reset.state_e60;v.world.speed_544=g->speed_544;
    c.driver.motion.orientation.settings=settings;
    *out=v;*context=c;return placed;
}



int taxi_simulation_start_challenge(TaxiDriveWorld *out,TaxiDriveWorldContext *context,
    const uint8_t *b,size_t size,const TaxiSpawnMarker *marker,uint8_t settings,
    unsigned submode,TaxiGroundQuery ground,void *query_context){
    if(!out||!context||(settings&224)!=128||submode>8)return -1;
    TaxiDriveWorld state;TaxiDriveWorldContext next;
    int result=taxi_simulation_start(&state,&next,b,size,marker,settings,ground,query_context);
    if(result!=1)return result;
    next.driver.motion.orientation.submode=(uint8_t)submode;
    *out=state;*context=next;return 1;
}

