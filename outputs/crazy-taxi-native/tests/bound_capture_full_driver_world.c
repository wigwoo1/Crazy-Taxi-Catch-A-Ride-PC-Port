#include "../src/drive_world.h"
int capture_full_driver(TaxiDriver*,TaxiDriverContext*,TaxiCommand*,const TaxiGearInput*,uint8_t,uint8_t,const TaxiTrig*,const TaxiAngleTable*,TaxiDriverEffects*);
int taxi_drive_world_step(TaxiDriveWorld *state,TaxiDriveWorldContext *context,
    TaxiCommand *command,const TaxiGearInput *input,uint8_t completion_result,
    const TaxiTrig *trig,const TaxiAngleTable *traction_angles,const TaxiNormalAngles *normal_angles,
    TaxiObstacleQuery obstacle,TaxiGroundQuery ground,void *query_context,TaxiDriveWorldEffects *effects) {
    if(!state||!context||!command||!input||!effects) return -1;
    TaxiDriveWorld v=*state;TaxiDriveWorldContext c=*context;TaxiCommand cmd=*command;TaxiDriveWorldEffects e={0};
    TaxiOrientation *o=&v.driver.motion.motion.orientation;TaxiWorldState *w=&v.world;
    o->position=w->height.position;o->heading_1c=w->heading_1c;o->heading_20=w->heading_20;
    o->drift_69c=w->drift_69c;o->angle_18=w->angle_18;o->angle_1a=w->angle_1a;
    if(capture_full_driver(&v.driver,&c.driver,&cmd,input,c.height.flag_1697,c.height.flag_153c,trig,traction_angles,&e.driver)) return -1;
    w->height.position=o->position;w->heading_1c=o->heading_1c;w->heading_20=o->heading_20;
    w->drift_69c=o->drift_69c;w->angle_18=o->angle_18;w->angle_1a=o->angle_1a;
    w->height.flags_28=o->flags_28;w->speed_544=o->speed_544;w->height.scale_e84=v.driver.power.scale_e84;
    TaxiPoint d=v.driver.motion.motion.displacement;
    w->displacement_x=d.x;w->height.displacement_y=d.y;w->displacement_z=d.z;
    TaxiHeightInput hi={.delta=input->delta,.settings=input->settings,
        .submode=c.driver.motion.orientation.submode,.completion_result=completion_result};
    if(taxi_world_update(w,&c.height,&hi,trig,normal_angles,obstacle,ground,query_context,&e.world)) return -1;
    o->position=w->height.position;o->heading_1c=w->heading_1c;o->heading_20=w->heading_20;
    o->drift_69c=w->drift_69c;o->angle_18=w->angle_18;o->angle_1a=w->angle_1a;
    o->flags_28=w->height.flags_28;o->speed_544=w->speed_544;
    v.driver.power.gears.flags_28=o->flags_28;v.driver.power.gears.speed_544=o->speed_544;
    v.driver.power.scale_e84=w->height.scale_e84;v.driver.motion.motion.scale_e84=w->height.scale_e84;
    *state=v;*context=c;*command=cmd;*effects=e;return 0;
}
