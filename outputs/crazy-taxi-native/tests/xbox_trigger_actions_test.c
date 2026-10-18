#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/xbox.h"
#include "../src/input.h"
#include "../src/actions.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
 const unsigned sequence[]={0,30,31,255,255,0,31,0};
 for(unsigned profile=0;profile<3;profile++)for(unsigned brake=0;brake<2;brake++){
  TaxiInput trigger={0},button={0};
  for(unsigned i=0;i<8;i++){
   unsigned value=sequence[i];TaxiXboxSample sample=taxi_xbox_decode(0,0,0,brake?0:value,brake?value:0);
   unsigned bit=profile==2?(brake?512:256):(brake?2:1);
   taxi_input_update(&trigger,(TaxiInputSample){.active_low_keys=taxi_xbox_driving_keys(sample,profile),.elapsed_ticks=4});
   taxi_input_update(&button,(TaxiInputSample){.active_low_keys=(uint16_t)(1023^(value>30?bit:0)),.elapsed_ticks=4});
   assert(!memcmp(&trigger,&button,sizeof trigger));
   for(unsigned action=0;action<7;action++)for(unsigned flags=0;flags<64;flags++){
    TaxiActionContext context={.profile=profile,.vehicle_flags=flags};
    assert(taxi_action_query(&trigger,context,action)==taxi_action_query(&button,context,action));
   }
   assert(sample.active_low_keys==1023);
  }
 }
 assert(taxi_xbox_driving_keys((TaxiXboxSample){0},0)==1023);
 puts("PASS: triggers and equivalent original buttons produce identical held, pressed, repeat and action histories in all three profiles; menu decode unchanged.");return 0;
}
