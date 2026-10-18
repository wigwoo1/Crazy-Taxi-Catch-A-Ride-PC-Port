#include "attract_entry.h"
#include "random.h"
int taxi_attract_entry(TaxiAttractEntry *s,uint8_t override_enabled,uint32_t *random){
 if(!s||!random||!override_enabled)return -1;
 s->scene_identity=0x02002634;s->settings[0]=(uint8_t)((s->settings[0]&31)|96);
 s->flag_153c=s->flag_1697=0;s->saved_city=s->settings[4];
 s->settings[4]=(uint8_t)(taxi_random_range(random,0,65535)&1);return 0;
}
