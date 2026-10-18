#include "attract_session.h"
int taxi_attract_session_begin(TaxiAttractSession *s,uint8_t settings[8],
 uint8_t driver,uint16_t city_random){
 if(!s||!settings)return -1;
 *s=(TaxiAttractSession){.active=1,.saved_city=settings[4],.driver=driver};
 taxi_attract_flow_begin(&s->flow);
 settings[0]=(uint8_t)((settings[0]&31)|0x60);
 settings[4]=(uint8_t)(city_random&1);
 return 0;
}
int taxi_attract_session_finish(TaxiAttractSession *s,uint8_t settings[8],
 uint8_t fade_active){
 if(!s||!settings)return -1;
 /* The recovered attract controller requests state2. State1 restarts AC70
    and is not the title-return transition represented by this owner. */
 if(!s->active||fade_active||s->flow.exit_gate!=2)return 0;
 settings[4]=s->saved_city;
 s->flow.exit_gate=0; /* B5c2 outer state clear before cleanup. */
 s->flow.elapsed=0;s->flow.blink_enabled=0; /* DEA8 preserves other fields. */
 s->active=0;
 return 1;
}
