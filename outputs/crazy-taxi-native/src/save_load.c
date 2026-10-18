#include "save_load.h"
#include "save_block.h"
#include <string.h>
static int call(TaxiSaveLoad *s,TaxiSaveLoadService service,void *context,uint32_t target,uint32_t a,uint32_t b,uint32_t *result){uint32_t ignored=0;return service(context,s,target,a,b,result?result:&ignored);}
static void restore_platform(TaxiSaveLoad *s){s->interrupt_callback=0x080161d9;s->wait_control=0x4014;}
static void defaults(TaxiSaveLoad *s){taxi_save_block_defaults(s->settings,s->runtime_rows,s->first_ten);s->music_volume=s->settings[1];s->effects_volume=s->settings[2];s->language=0;}
int taxi_save_load(TaxiSaveLoad *s,uint32_t slot,uint32_t fallback,TaxiSaveLoadService service,void *context){
 if(!s||!s->settings||!s->runtime_rows||!s->first_ten||!service)return -1;
 slot&=65535;fallback&=255;uint8_t retained[512];memcpy(retained,s->settings,512);s->preserve_settings=1;s->sectors_checked=s->attempts=0;
 if(call(s,service,context,0x0802763c,0,0,0)||call(s,service,context,0x08027730,0,0,0)||call(s,service,context,0x08027558,0,0,0))return -1;
 s->music_enabled=s->effects_enabled=0;
 if(call(s,service,context,0x08016208,5,0,0)||call(s,service,context,0x0803203c,4,0,0)||call(s,service,context,0x080320a4,2,0x02003548,0)||call(s,service,context,0x08016208,5,0,0))return -1;
 uint32_t status=0;
 for(unsigned sector=0;sector<64;sector++){
  s->sectors_checked++;s->device_block=s->settings+sector*8;
  for(unsigned attempt=0;attempt<50;attempt++){
   s->attempts++;
   if(call(s,service,context,0x08032228,sector,0,0)||call(s,service,context,0x080323b4,sector,0,&status))return -1;
   status&=65535;if(!status)break;
  }
  if(status&0x8000){
   restore_platform(s);
   if(fallback)defaults(s);
   else {if(call(s,service,context,0x080105bc,0,slot,0))return -1;memcpy(s->settings,retained,512);}
   break;
  }
 }
 if(!taxi_save_block_checksum(s->settings)||!taxi_save_block_checksum_valid(s->settings)){
  restore_platform(s);
  if(fallback)defaults(s);
  else {if(call(s,service,context,0x080105bc,0,slot,0))return -1;memcpy(s->settings,retained,512);}
 }
 if(call(s,service,context,0x08026630,2,0,0))return -1;
 s->music_enabled=s->effects_enabled=1;s->music_volume=s->settings[1];s->effects_volume=s->settings[2];
 if(!fallback){
  /* The caller resolves original table087EC048 and byte087EC064. */
  if(call(s,service,context,0x08026ef8,s->music_module,0,0)||call(s,service,context,0x08027110,s->start_order,0,0))return -1;
 }
 restore_platform(s);memcpy(s->runtime_rows,s->settings+424,80);s->preserve_settings=0;s->settings[0]&=(uint8_t)~1u;s->delta=1;s->previous_clock=s->current_clock;return 0;
}
