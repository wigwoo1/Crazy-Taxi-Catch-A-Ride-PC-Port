#include "startup_init.h"
#include <string.h>
int taxi_startup_init_begin(TaxiStartupInit *s,TaxiStartupInitCall call,void *context){
 if(!s||!call)return -1;
 if(!s->retained_1ea5){
  if(call(context,s,0x0800ffe0,0,0))return -1;
  memset(s->font_assets,0,sizeof s->font_assets);
  if(call(context,s,0x08028f64,0,0))return -1;
  s->font_assets[5]=0x0849dfe0;
  if(call(context,s,0x08028fb8,5,0x0849dfe0))return -1;
  s->selected_font=5;
  if(call(context,s,0x08028fa0,5,0))return -1;
  if(call(context,s,0x08016250,0,0))return -1;
  if(call(context,s,0x08016100,0,0))return -1;
  memset(s->font_assets,0,sizeof s->font_assets);
  if(call(context,s,0x08028f64,0,0))return -1;
  s->font_assets[5]=0x0849dfe0;
  if(call(context,s,0x08028fb8,5,0x0849dfe0))return -1;
  if(!s->retained_1ea5){
   if(call(context,s,0x0800ffe0,0,0))return -1;
   if(call(context,s,0x08010298,1,1))return -1;
  }
 }
 s->initialized_1ea4=1;s->dispatcher_3878=0x0801e275;s->child_signal_3892=0;
 return 0;
}
