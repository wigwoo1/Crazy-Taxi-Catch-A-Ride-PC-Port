#include "driver_choice.h"
void taxi_driver_choice_begin(TaxiDriverChoice *s) { *s=(TaxiDriverChoice){0,0}; }
unsigned taxi_driver_choice_step(TaxiDriverChoice *s,const TaxiMenuInput *in,int fade_active) {
    if(!s||!in||s->driver>3||s->exiting||fade_active)return 0;
    if(in->pressed[0]||in->pressed[3]) {s->exiting=1;return TAXI_DRIVER_CONFIRM;}
    if(in->pressed[1]) {s->exiting=2;return TAXI_DRIVER_BACK;}
    if(in->pressed[5]&&s->driver>0) {s->driver--;return TAXI_DRIVER_MOVED;}
    if(in->pressed[4]&&s->driver<3) {s->driver++;return TAXI_DRIVER_MOVED;}
    return 0;
}
