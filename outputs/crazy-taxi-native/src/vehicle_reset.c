#include "vehicle_reset.h"
#include <string.h>
void taxi_vehicle_reset(TaxiVehicleReset *v,uint8_t settings,int32_t *state_2630) {
    TaxiOrientation o=v->orientation;
    memset(v,0,sizeof *v);v->orientation=o;
    v->orientation.drift_69c=0;v->orientation.timer_2a=0;
    v->orientation.flags_28&=31;v->orientation.phase_e6a=0;
    v->state_e60=65535;v->state_64c=2;
    v->state_654=o.heading_20;
    if((settings&224)!=128) {
        /* Low 12 bits of ASR12 equal low 12 bits of logical shift12. */
        v->state_654=(int32_t)(((((uint32_t)o.heading_20>>12)+2048)&4095)<<12);
    }
    v->state_611=v->state_612=v->state_613=128;
    v->state_614=v->state_620=3000;
    v->state_754=57344;v->state_758=28672;v->state_75c=-1;
    *state_2630=0;
}
