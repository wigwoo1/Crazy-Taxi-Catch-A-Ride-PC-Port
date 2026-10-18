#include "world_update.h"
#include "vehicle_math.h"
#include <string.h>
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
static int32_t add(int32_t a,int32_t b) {return bits((uint32_t)a+(uint32_t)b);}
static int32_t sub(int32_t a,int32_t b) {return bits((uint32_t)a-(uint32_t)b);}
static int32_t asr(int32_t a,unsigned n) {return bits(((uint32_t)a>>n)|(a<0?(UINT32_MAX<<(32-n)):0));}
static uint8_t color(uint8_t old,uint8_t target) {
    int32_t d=asr((target>>1)-(int32_t)old,3);return (uint8_t)(old+(d?d:1));
}
int taxi_world_update(TaxiWorldState *out,TaxiHeightGlobals *global_out,
    const TaxiHeightInput *in,const TaxiTrig *trig,const TaxiNormalAngles *angles,
    TaxiObstacleQuery obstacle,TaxiGroundQuery ground,void *context,TaxiWorldEffects *effects) {
    if(!out||!global_out||!in||!trig||!angles||!obstacle||!ground||!effects) return -1;
    TaxiWorldState v=*out;TaxiHeightGlobals g=*global_out;TaxiWorldEffects e={0};
    TaxiVehicleBody body={.position=v.height.position,.state_e84=v.height.scale_e84,
        .impact={.angle_18=v.angle_18,.angle_1a=v.angle_1a,.angle_1c=v.heading_1c,
            .angle_69c=v.drift_69c,.speed_544=v.speed_544,
            .state_748=v.height.body.velocity_748,.state_73c=v.height.body.velocity_73c,
            .state_74c=v.impulse_74c,.state_750=v.impulse_750,.state_34=v.impulse_34}};
    TaxiPoint move={add(v.displacement_x,asr(v.impulse_74c,12)),v.height.displacement_y,
        add(v.displacement_z,asr(v.impulse_750,12))};
    if(taxi_move_vehicle(&body,move,in->settings,in->submode,trig,obstacle,context,&e.movement,&e.impact)) return -1;
    v.height.position=body.position;v.height.scale_e84=body.state_e84;
    v.height.body.velocity_748=body.impact.state_748;v.height.body.velocity_73c=body.impact.state_73c;
    v.speed_544=body.impact.speed_544;
    int32_t turn=bits((uint32_t)body.impact.state_34*(uint32_t)in->delta)/60;
    v.heading_1c=add(v.heading_1c,turn);v.heading_20=add(v.heading_20,turn);
    v.impulse_74c=taxi_vehicle_damp(body.impact.state_74c);
    v.impulse_750=taxi_vehicle_damp(body.impact.state_750);
    v.impulse_34=taxi_vehicle_damp(body.impact.state_34);
    TaxiWorldContact c={.red=(uint8_t)(v.red_374<<1),.green=(uint8_t)(v.green_375<<1),.blue=(uint8_t)(v.blue_376<<1)};
    TaxiGroundHit hit={.height=INT32_MIN};
    int result=ground(context,(TaxiPoint){v.height.position.x,v.ground_36c,v.height.position.z},&hit);
    if(result<0||result>1) return -1;
    e.ground_hit=result&&taxi_world_contact(&c,&hit);
    TaxiVehicleMath scalar={.y=v.height.position.y,.state_698=v.height.state_698,
        .state_40=v.state_40,.state_44=v.state_44,.state_544=v.speed_544,
        .state_2c=v.state_2c,.flags_28=v.height.flags_28};
    if(e.ground_hit) taxi_vehicle_vertical_step(&scalar,in->delta);
    taxi_vehicle_contact_decay(&scalar);
    v.state_40=scalar.state_40;v.state_44=scalar.state_44;v.speed_544=scalar.state_544;v.state_2c=scalar.state_2c;
    if(e.ground_hit) {
        v.height.position.y=scalar.y;v.height.state_698=scalar.state_698;v.material_e60=c.material;
        v.red_374=color(v.red_374,c.red);v.green_375=color(v.green_375,c.green);v.blue_376=color(v.blue_376,c.blue);
        v.previous_37c=v.contact_378;v.contact_378=c.contact_value;v.ground_36c=c.height;
        v.height.flags_28=(uint8_t)((v.height.flags_28&~16)|((c.flags&1)<<4));
        if(taxi_height_update(&v.height,&g,&c,in,angles,&e.height)) return -1;
        int32_t angle=asr(sub(0,add(v.heading_1c,v.drift_69c)),12);
        TaxiPoint p=taxi_rotate_y(v.height.body.orientation_6f4,angle,trig);
        v.angle_38=(uint16_t)p.x;v.angle_3a=(uint16_t)p.z;v.height_3c=c.height;
        v.angle_18=(uint16_t)sub(p.x,v.height.body.response_704);
        v.angle_1a=(uint16_t)add(p.z,v.height.body.response_70c);v.snapshot_714=c;
    } else {v.material_e60=65535;v.height.state_698=0;}
    *out=v;*global_out=g;*effects=e;return 0;
}

