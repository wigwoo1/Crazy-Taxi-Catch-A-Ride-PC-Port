#ifdef NDEBUG
#undef NDEBUG
#endif
#include "attract_session.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
 for(unsigned mode=0;mode<256;mode++)for(unsigned city=0;city<256;city++){
  uint8_t settings[8]={mode,7,8,9,city,11,12,13},before[8];
  memcpy(before,settings,8);
  TaxiAttractSession s;
  assert(!taxi_attract_session_begin(&s,settings,3,(uint16_t)(mode*256+city)));
  assert(s.active&&s.driver==3&&s.saved_city==city);
  assert(settings[0]==((mode&31)|0x60)&&settings[4]==(city&1));
  for(unsigned i=1;i<8;i++)if(i!=4)assert(settings[i]==before[i]);
  assert(!taxi_attract_session_finish(&s,settings,0));
  s.flow.exit_gate=1;
  assert(!taxi_attract_session_finish(&s,settings,0)&&s.active);
  s.flow.exit_gate=0;
  assert(taxi_attract_flow_step(&s.flow,1,0,1,0)==1);
  assert(!taxi_attract_session_finish(&s,settings,1)&&s.active);
  assert(taxi_attract_session_finish(&s,settings,0)==1);
  assert(!s.active&&!s.flow.elapsed&&!s.flow.blink_enabled&&s.flow.exit_state==1);
  assert(settings[4]==city&&settings[0]==((mode&31)|0x60));
  assert(!taxi_attract_session_finish(&s,settings,0));
 }
 TaxiAttractSession s;uint8_t settings[8]={0,0,0,0,1};
 assert(!taxi_attract_session_begin(&s,settings,2,65535));
 for(int i=0;i<9020;i++)assert(taxi_attract_flow_step(&s.flow,1,0,0,0)==(i==9019));
 assert(!taxi_attract_session_finish(&s,settings,1));
 assert(taxi_attract_session_finish(&s,settings,0)==1&&settings[4]==1);
 assert(taxi_attract_session_begin(0,settings,0,0)==-1);
 assert(taxi_attract_session_finish(&s,0,0)==-1);
 puts("PASS attract session: settings preservation, random city low bit, retained driver, cancellation/natural timeout, fade gate, city restore and one-shot cleanup; host transition pending.");
}
