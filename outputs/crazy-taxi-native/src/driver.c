#include "driver.h"
#include "free_rev.h"
#include "driver_tail.h"
#include "approach.h"
#include <string.h>
static int32_t bits(uint32_t u){int32_t v;memcpy(&v,&u,4);return v;}
static int emit_sounds(const TaxiGearEffects *e,unsigned phase,
    TaxiDriverSoundObserver observer,void *context){
    if(!observer)return 0;
    for(unsigned j=0;j<e->count;j++)if(observer(context,phase,e->indices[j]))return -1;
    return 0;
}
int taxi_driver_step_observed(TaxiDriver *state,TaxiDriverContext *context,TaxiCommand *command,
    const TaxiGearInput *input,uint8_t flag_1697,uint8_t flag_153c,
    const TaxiTrig *trig,const TaxiAngleTable *table,TaxiDriverEffects *effects,
    TaxiDriverSoundObserver observer,void *observer_context) {
    if(!state||!context||!command||!input||!trig||!table||!effects) return -1;
    TaxiDriver v=*state;TaxiDriverContext c=*context;TaxiCommand cmd=*command;
    TaxiDriverEffects e={0};TaxiGearInput in=*input;
    c.gear.state_2630=taxi_driver_countdown(c.gear.state_2630,in.frame_delta);
    c.power.state_2630=c.gear.state_2630;c.motion.traction.state_2630=c.gear.state_2630;
    TaxiGearState *g=&v.power.gears;TaxiOrientation *o=&v.motion.motion.orientation;
    taxi_steering_input(g,&cmd,v.steering_rate_57a,in.delta,c.freeze_2620,c.gear.state_2630);
    in.command_flags=cmd.bytes[4];in.freeze_2620=c.freeze_2620;
    if((!g->gear&&!(o->flags_358&1))||(g->flags_28&8)) {
        if(taxi_free_rev(&v.power,in.delta,cmd.bytes[4],&e.power)||taxi_gear_clamp(g)) return -1;
        if(emit_sounds(&e.power,1,observer,observer_context))return -1;
        g->boost_75c=g->rpm_538;
    } else {
        if(!g->gear&&(o->flags_358&1)) g->gear=1;
        if(taxi_gear_update(g,&in,&c.gear,&e.gear)) return -1;
        if(emit_sounds(&e.gear,0,observer,observer_context))return -1;
        c.power.state_2630=c.gear.state_2630;c.power.state_1654=c.gear.state_1654;
        TaxiAccelerationInput ai={.delta=in.delta,.frame_delta=in.frame_delta,.settings=in.settings,
            .flag_1697=flag_1697,.flag_153c=flag_153c,.action2=in.action2};
        /* This driver wrapper represents player index 0, matching its single actor. */
        if(taxi_acceleration_update(&v.power,&ai,&c.power,trig,&e.power)) return -1;
        if(emit_sounds(&e.power,1,observer,observer_context))return -1;
        c.gear.state_2630=c.power.state_2630;c.gear.state_1654=c.power.state_1654;
        TaxiBrakeInput bi={.delta=in.delta,.settings=in.settings,.flag_1697=flag_1697,
            .flag_153c=flag_153c,.action2=in.action2,.action3=in.action3,.sound_enabled=in.sound_enabled};
        if(taxi_brake_update(&v.power,&bi,&c.freeze_2620,&e.brake))return -1;
        if(e.brake.sound_requested&&observer&&
            observer(observer_context,2,e.brake.sound_index))return -1;
        if(taxi_gear_clamp(g))return -1;
    }
    o->speed_544=g->speed_544;o->flags_28=g->flags_28;o->phase_e6a=g->phase_e6a;
    o->force_540=v.power.force_540;
    v.motion.motion.steering.angle_578=(uint16_t)g->steering_578;
    v.motion.motion.steering.phase_e6b=g->phase_e6b;
    v.motion.motion.scale_e84=v.power.scale_e84;v.motion.force_53c=v.power.response_53c;
    v.motion.flags_4ac=g->flags_4ac;
    c.motion.orientation.delta=in.delta;c.motion.orientation.settings=in.settings;
    c.motion.orientation.flag_1697=flag_1697;c.motion.orientation.amount_1654=c.gear.state_1654;
    c.motion.traction.state_2630=c.gear.state_2630;c.motion.traction.flag_25d4=c.gear.state_25d4;
    if(taxi_vehicle_update(&v.motion,&c.motion,&cmd,trig,table,&e.motion)) return -1;
    int32_t rpm_step=bits((uint32_t)in.delta<<8);
    v.rpm_53a=g->gear?(int16_t)taxi_approach_s32(v.rpm_53a,g->rpm_538,rpm_step):g->rpm_538;
    g->speed_544=o->speed_544;g->flags_28=o->flags_28;g->phase_e6a=o->phase_e6a;
    v.power.force_540=o->force_540;
    c.gear.state_1654=c.motion.orientation.amount_1654;c.gear.state_25d4=c.motion.traction.flag_25d4;
    taxi_driver_effect_decay(&v.power,in.delta,in.freeze_177b);
    taxi_driver_tail(&v.power,in.frame_delta,in.action2,in.action3);
    v.motion.motion.scale_e84=v.power.scale_e84;
    *state=v;*context=c;*command=cmd;*effects=e;return 0;
}

int taxi_driver_step(TaxiDriver *state,TaxiDriverContext *context,TaxiCommand *command,
    const TaxiGearInput *input,uint8_t flag_1697,uint8_t flag_153c,
    const TaxiTrig *trig,const TaxiAngleTable *table,TaxiDriverEffects *effects){
    return taxi_driver_step_observed(state,context,command,input,flag_1697,
        flag_153c,trig,table,effects,NULL,NULL);
}
