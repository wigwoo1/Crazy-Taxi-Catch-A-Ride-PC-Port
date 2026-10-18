#include "startup_boot.h"
int taxi_startup_boot_begin(TaxiStartupBoot *s,TaxiStartupBootCall call,void *context){
 if(!s||!s->init||!s->audio||!s->music_context||!call)return -1;
 s->init->initialized_1ea4=0;s->audio->processing=0;
 if(call(context,s,0x0801e2e4,0))return -1;
 if(call(context,s,0x08010a54,0))return -1;
 if(call(context,s,0x08026488,2))return -1;
 if(call(context,s,0x08016208,10))return -1;
 if(!s->init->retained_1ea5)s->audio->music_enabled=s->audio->effects_enabled=1;
 s->music_context->main_mode=1;return 0;
}
