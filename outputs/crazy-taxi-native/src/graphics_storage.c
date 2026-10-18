#include "graphics_storage.h"
#include <string.h>
int taxi_graphics_storage_reset(TaxiGraphicsStorage *s){
 if(!s)return -1;
 s->state_6844=s->background_6850=s->state_6904=0;
 memset(s->pool_1b6ec,0,sizeof s->pool_1b6ec);
 memset(s->pool_276ec,0,sizeof s->pool_276ec);
 memset(s->heads_2b6ec,0,sizeof s->heads_2b6ec);
 return 0;
}
