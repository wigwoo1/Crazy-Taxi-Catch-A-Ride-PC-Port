#include "drive_world.h"
/* B284: actor zero gear update, then its 11F80 world update. Other actors
   remain the caller's responsibility in original 15AC8 dispatcher order. */
int taxi_drive_world_start_step(TaxiDriveWorld *state,TaxiDriveWorldContext *context,
    const TaxiGearInput *input,uint8_t completion_result,const TaxiTrig *trig,
    const TaxiNormalAngles *angles,TaxiObstacleQuery obstacle,TaxiGroundQuery ground,
    void *query_context,TaxiDriveWorldEffects *effects) {
    if(!state||!context||!input||!effects) return -1;
    TaxiDriveWorld v=*state;TaxiDriveWorldContext c=*context;TaxiDriveWorldEffects e={0};
    TaxiGearState *gear=&v.driver.power.gears;TaxiWorldState *w=&v.world;
    if(taxi_gear_update(gear,input,&c.driver.gear,&e.driver.gear)) return -1;
    w->speed_544=gear->speed_544;w->height.flags_28=gear->flags_28;
    TaxiHeightInput hi={.delta=input->delta,.settings=input->settings,
        .submode=c.driver.motion.orientation.submode,.completion_result=completion_result};
    if(taxi_world_update(w,&c.height,&hi,trig,angles,obstacle,ground,query_context,&e.world)) return -1;
    TaxiOrientation *o=&v.driver.motion.motion.orientation;
    o->position=w->height.position;o->heading_1c=w->heading_1c;o->heading_20=w->heading_20;
    o->drift_69c=w->drift_69c;o->angle_18=w->angle_18;o->angle_1a=w->angle_1a;
    o->flags_28=w->height.flags_28;o->speed_544=w->speed_544;
    gear->flags_28=w->height.flags_28;gear->speed_544=w->speed_544;
    v.driver.power.scale_e84=w->height.scale_e84;v.driver.motion.motion.scale_e84=w->height.scale_e84;
    *state=v;*context=c;*effects=e;return 0;
}

static FILE *held_gear_trace;
void taxi_drive_world_gear_trace(FILE *sink){held_gear_trace=sink;}
static void held_gear_state(const TaxiGearState *g,const TaxiGearGlobals *globals){
 fprintf(held_gear_trace," %ld",(long)g->flags_4ac);
 fprintf(held_gear_trace," %ld",(long)g->flags_28);
 fprintf(held_gear_trace," %ld",(long)g->max_gear);
 fprintf(held_gear_trace," %ld",(long)g->ratios[0]);
 fprintf(held_gear_trace," %ld",(long)g->ratios[1]);
 fprintf(held_gear_trace," %ld",(long)g->ratios[2]);
 fprintf(held_gear_trace," %ld",(long)g->ratios[3]);
 fprintf(held_gear_trace," %ld",(long)g->ratios[4]);
 fprintf(held_gear_trace," %ld",(long)g->ratios[5]);
 fprintf(held_gear_trace," %ld",(long)g->ratios[6]);
 fprintf(held_gear_trace," %ld",(long)g->reverse_ratio);
 fprintf(held_gear_trace," %ld",(long)g->final_ratio);
 fprintf(held_gear_trace," %ld",(long)g->gear);
 fprintf(held_gear_trace," %ld",(long)g->next_gear);
 fprintf(held_gear_trace," %ld",(long)g->shift_timer);
 fprintf(held_gear_trace," %ld",(long)g->min_530);
 fprintf(held_gear_trace," %ld",(long)g->max_532);
 fprintf(held_gear_trace," %ld",(long)g->up_534);
 fprintf(held_gear_trace," %ld",(long)g->down_536);
 fprintf(held_gear_trace," %ld",(long)g->rpm_538);
 fprintf(held_gear_trace," %ld",(long)g->speed_544);
 fprintf(held_gear_trace," %ld",(long)g->speed_568);
 fprintf(held_gear_trace," %ld",(long)g->scale_5b8);
 fprintf(held_gear_trace," %ld",(long)g->scale_5cc);
 fprintf(held_gear_trace," %ld",(long)g->boost_75c);
 fprintf(held_gear_trace," %ld",(long)g->steering_578);
 fprintf(held_gear_trace," %ld",(long)g->state_e68);
 fprintf(held_gear_trace," %ld",(long)g->state_e69);
 fprintf(held_gear_trace," %ld",(long)g->phase_e6a);
 fprintf(held_gear_trace," %ld",(long)g->phase_e6b);
 fprintf(held_gear_trace," %ld",(long)g->timer_e6c);
 fprintf(held_gear_trace," %ld",(long)g->timer_e70);
 fprintf(held_gear_trace," %ld",(long)g->timer_e78);
 fprintf(held_gear_trace," %ld",(long)g->timer_e7c);
 fprintf(held_gear_trace," %ld",(long)g->timer_e80);
 fprintf(held_gear_trace," %ld %ld %u",(long)globals->state_2630,(long)globals->state_1654,globals->state_25d4);
}
int taxi_drive_world_gear_step(TaxiDriveWorld *state,TaxiDriveWorldContext *context,const TaxiGearInput *input,TaxiDriveWorldEffects *effects){
 if(!state||!context||!input||!effects)return -1;
 TaxiDriveWorld v=*state;TaxiDriveWorldContext c=*context;TaxiDriveWorldEffects e={0};TaxiGearInput in=*input;in.command_flags=0;
 if(held_gear_trace){
  fprintf(held_gear_trace," %ld",(long)in.action2);
  fprintf(held_gear_trace," %ld",(long)in.action3);
  fprintf(held_gear_trace," %ld",(long)in.action4);
  fprintf(held_gear_trace," %ld",(long)in.action5);
  fprintf(held_gear_trace," %ld",(long)in.settings);
  fprintf(held_gear_trace," %ld",(long)in.freeze_2620);
  fprintf(held_gear_trace," %ld",(long)in.freeze_177b);
  fprintf(held_gear_trace," %ld",(long)in.sound_enabled);
  fprintf(held_gear_trace," %ld",(long)in.delta);
  fprintf(held_gear_trace," %ld",(long)in.frame_delta);
  fprintf(held_gear_trace," %ld",(long)in.held[0]);
  fprintf(held_gear_trace," %ld",(long)in.held[1]);
  fprintf(held_gear_trace," %ld",(long)in.held[2]);
  fprintf(held_gear_trace," %ld",(long)in.held[3]);
  fprintf(held_gear_trace," %ld",(long)in.held[4]);
  fprintf(held_gear_trace," %ld",(long)in.held[5]);
  fprintf(held_gear_trace," %ld",(long)in.held[6]);
  fprintf(held_gear_trace," %ld",(long)in.held[7]);
  fprintf(held_gear_trace," %ld",(long)in.held[8]);
  fprintf(held_gear_trace," %ld",(long)in.held[9]);
  held_gear_state(&v.driver.power.gears,&c.driver.gear);
 }
 if(taxi_gear_update(&v.driver.power.gears,&in,&c.driver.gear,&e.driver.gear))return -1;
 v.world.speed_544=v.driver.power.gears.speed_544;v.world.height.flags_28=v.driver.power.gears.flags_28;
 v.driver.motion.motion.orientation.speed_544=v.driver.power.gears.speed_544;
 v.driver.motion.motion.orientation.flags_28=v.driver.power.gears.flags_28;
 c.driver.power.state_2630=c.driver.gear.state_2630;c.driver.power.state_1654=c.driver.gear.state_1654;
 c.driver.motion.orientation.amount_1654=c.driver.gear.state_1654;c.driver.motion.traction.state_2630=c.driver.gear.state_2630;c.driver.motion.traction.flag_25d4=c.driver.gear.state_25d4;
 if(held_gear_trace){held_gear_state(&v.driver.power.gears,&c.driver.gear);fprintf(held_gear_trace," %u %u %u %d\n",e.driver.gear.count,e.driver.gear.indices[0],e.driver.gear.indices[1],e.driver.gear.invalid_mode);}
 *state=v;*context=c;*effects=e;return 0;
}
