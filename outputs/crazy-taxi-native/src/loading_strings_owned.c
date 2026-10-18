#include "loading_strings_owned.h"
#include "string_lookup.h"
static int call(void *context,TaxiLoadingPresent *s,uint32_t target,const uint32_t a[9],uint32_t position[3],uint32_t *result){
 const TaxiLoadingStringsOwner *o=context;
 if(target==0x08028fe4)return taxi_string_lookup(o->rom,o->rom_size,*o->selected_table,a[0],result);
 return o->service(o->context,s,target,a,position,result);
}
int taxi_loading_strings_owned(TaxiLoadingPresent *s,const TaxiLoadingStringsOwner *o){
 if(!s||!o||!o->rom||!o->selected_table||!o->service)return -1;
 return taxi_loading_present(s,call,(void *)o);
}
