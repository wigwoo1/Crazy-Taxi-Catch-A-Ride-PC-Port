#include "loading_present_bypass.h"
int taxi_loading_present_bypass(TaxiLoadingPresentGate *s,TaxiLoadingClearCall clear,void *context){
 if(!s||!clear)return -1;
 const uint32_t args[4]={0,0,s->width_7542,s->height_7544};
 if(clear(context,s,0x0802c9f0,args))return -1;
 if((s->settings_1eb0&0xe0)==0x80)return 1;
 return s->override_1708?1:0;
}
