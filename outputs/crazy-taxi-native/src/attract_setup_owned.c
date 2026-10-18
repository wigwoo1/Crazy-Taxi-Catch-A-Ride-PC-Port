#include "attract_setup_owned.h"
int taxi_attract_setup_owned(TaxiAttractSetup *s,TaxiNormalRestartOwners *music,TaxiRestartResources *resources,TaxiAttractGraphicsOwners *graphics){
 if(!s||!music||!resources||!graphics||!resources->palette||!resources->display||!resources->bases||graphics->palette!=resources->palette)return -1;
 s->flag_1ea6=0;s->retained_sl=1;s->controller_identity=0x02001790;
 TaxiNormalRestartOwners owner=*music;owner.resource=taxi_restart_resources_call;owner.context=resources;
 if(taxi_normal_restart_owned(&s->prefix,&owner))return -1;
 graphics->destination=resources->bases->base_1530;
 return taxi_attract_graphics_owned(graphics);
}
