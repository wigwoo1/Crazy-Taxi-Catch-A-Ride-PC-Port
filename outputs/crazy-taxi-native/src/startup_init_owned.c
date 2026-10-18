#include "startup_init_owned.h"
#include "save_block.h"
#include <string.h>
typedef struct {const TaxiStartupInitOwners *owners;TaxiStartupInitChild child;void *context;} OwnedContext;
static int service(void *opaque,TaxiStartupInit *s,uint32_t target,uint32_t a,uint32_t b){
 OwnedContext *c=opaque;const TaxiStartupInitOwners *o=c->owners;
 if(target==0x0800ffe0){
  taxi_save_block_defaults(o->settings,o->runtime_rows,o->first_ten);
  *o->music_volume=o->settings[1];*o->effects_volume=o->settings[2];*o->language=0;return 0;
 }
 if(target==0x08016100){taxi_input_startup_reset(o->input);return 0;}
 /* Font slots and selected identity are already updated by startup_init;
    these requests do not load invented glyph assets. */
 if(target==0x08028f64)return 0;
 if(target==0x08028fb8)return a==5&&b==0x0849dfe0&&s->font_assets[5]==b?0:-1;
 if(target==0x08028fa0)return a==5&&s->selected_font==5?0:-1;
 if(target==0x08016250||target==0x08010298)return c->child(c->context,s,target,a,b);
 return -1;
}
int taxi_startup_init_owned(TaxiStartupInit *s,const TaxiStartupInitOwners *o,TaxiStartupInitChild child,void *context){
 if(!s||!o||!o->settings||!o->runtime_rows||!o->first_ten||!o->input||!o->music_volume||!o->effects_volume||!o->language||!child)return -1;
 OwnedContext c={o,child,context};return taxi_startup_init_begin(s,service,&c);
}

static int owners_valid(const TaxiStartupInitOwners *o){
 return o&&o->settings&&o->runtime_rows&&o->first_ten&&o->input&&o->music_volume&&o->effects_volume&&o->language;
}
static void apply_defaults(const TaxiStartupInitOwners *o){
 taxi_save_block_defaults(o->settings,o->runtime_rows,o->first_ten);
 *o->music_volume=o->settings[1];*o->effects_volume=o->settings[2];*o->language=0;
}
static void register_font(TaxiStartupInit *s){
 memset(s->font_assets,0,sizeof s->font_assets);s->font_assets[5]=0x0849dfe0;
}
static int finish(TaxiStartupInitContinuation *c){
 c->init->initialized_1ea4=1;c->init->dispatcher_3878=0x0801e275;c->init->child_signal_3892=0;
 c->pending=TAXI_STARTUP_INIT_DONE;return TAXI_STARTUP_INIT_DONE;
}
int taxi_startup_init_owned_begin(TaxiStartupInitContinuation *c,TaxiStartupInit *s,const TaxiStartupInitOwners *o){
 if(!c||!s||!owners_valid(o))return -1;
 c->init=s;c->owners=*o;c->pending=TAXI_STARTUP_INIT_DONE;
 if(s->retained_1ea5)return finish(c);
 apply_defaults(o);register_font(s);s->selected_font=5;
 c->pending=TAXI_STARTUP_INIT_SCREEN;return c->pending;
}
int taxi_startup_init_owned_resume(TaxiStartupInitContinuation *c){
 if(!c||!c->init||!owners_valid(&c->owners))return -1;
 if(c->pending==TAXI_STARTUP_INIT_SAVE)return finish(c);
 if(c->pending!=TAXI_STARTUP_INIT_SCREEN)return -1;
 taxi_input_startup_reset(c->owners.input);register_font(c->init);
 /* Original second font registration does not select it again. */
 if(c->init->retained_1ea5)return finish(c);
 apply_defaults(&c->owners);c->pending=TAXI_STARTUP_INIT_SAVE;return c->pending;
}
