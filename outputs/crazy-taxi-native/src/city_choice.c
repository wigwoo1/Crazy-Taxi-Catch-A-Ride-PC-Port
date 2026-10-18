#include "city_choice.h"
void taxi_city_choice_begin(TaxiCityChoice *s,uint8_t settings[8]) {
    if(!s||!settings)return;
    s->exiting=0;settings[4]=0;
}
unsigned taxi_city_choice_step(TaxiCityChoice *s,uint8_t settings[8],const TaxiMenuInput *in,int fade_active) {
    if(!s||!settings||!in||settings[4]>1||s->exiting||fade_active)return 0;
    if(in->pressed[0]||in->pressed[3]) {
        if(settings[4]==1&&!settings[7])return TAXI_CITY_LOCKED;
        s->exiting=1;return TAXI_CITY_CONFIRM;
    }
    if(in->pressed[1]) {s->exiting=2;return TAXI_CITY_BACK;}
    if(settings[4]) {
        if(in->pressed[5]) {settings[4]=0;return TAXI_CITY_MOVED;}
    } else if(in->pressed[4]) {settings[4]=1;return TAXI_CITY_MOVED;}
    return 0;
}
