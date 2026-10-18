#include "vehicle_update.h"
#include "vehicle_math.h"
int taxi_vehicle_update(TaxiVehicleUpdate *state,TaxiVehicleUpdateContext *context,
    const TaxiCommand *command,const TaxiTrig *trig,const TaxiAngleTable *table,
    TaxiVehicleUpdateEffects *effects) {
    if(!state||!context||!command||!trig||!table||!effects) return -1;
    TaxiVehicleUpdate v=*state;TaxiVehicleUpdateContext c=*context;
    TaxiVehicleUpdateEffects e={0};
    if(taxi_motion_prepare(&v.motion,&c.orientation,command,trig,&e.orientation)) return -1;
    TaxiOrientation *o=&v.motion.orientation;
    TaxiTraction body={.force_53c=v.force_53c,.force_540=o->force_540,
        .capacity_570=v.capacity_570,.capacity_574=v.capacity_574,
        .speed_544=o->speed_544,.state_564=o->state_564,.flags_28=o->flags_28,
        .phase_e6a=o->phase_e6a,.phase_e6b=v.motion.steering.phase_e6b};
    c.traction.settings=c.orientation.settings;c.traction.flag_1697=c.orientation.flag_1697;
    if(taxi_traction_step(&body,&c.traction,trig,table,&e.traction)) return -1;
    o->speed_544=body.speed_544;o->state_564=body.state_564;
    TaxiVehicleMath response={.state_540=o->force_540,.state_744=v.response_744,.flags_4ac=v.flags_4ac};
    taxi_vehicle_response_step(&response);
    o->force_540=response.state_540;v.response_744=response.state_744;
    *state=v;*context=c;*effects=e;return 0;
}
