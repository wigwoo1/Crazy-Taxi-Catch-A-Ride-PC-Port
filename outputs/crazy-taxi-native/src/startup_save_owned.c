#include "startup_save_owned.h"
int taxi_startup_save_owned(TaxiStartupInitContinuation *c,TaxiSaveLoad *s,TaxiSaveLoadOwners *o){
 if(!c||!c->init||c->pending!=TAXI_STARTUP_INIT_SAVE||!s||
    c->owners.settings!=s->settings||c->owners.runtime_rows!=s->runtime_rows||c->owners.first_ten!=s->first_ten||
    !c->owners.input||!c->owners.music_volume||!c->owners.effects_volume||!c->owners.language)return -1;
 if(taxi_save_load_owned(s,1,o))return -1;
 *c->owners.music_volume=s->music_volume;*c->owners.effects_volume=s->effects_volume;*c->owners.language=s->language;
 return taxi_startup_init_owned_resume(c);
}
