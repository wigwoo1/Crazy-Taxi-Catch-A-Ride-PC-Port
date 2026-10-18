#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "passenger_init.h"
int main(void){
 TaxiPassengerState state,before;TaxiPassengerResolved r={.kind=0,.origin={87770,-3},.spawn_height=12345};
 static const unsigned written[]={0,1,8,9,10,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35};
 for(unsigned fill=0;fill<256;fill++){
  memset(&state,fill,sizeof state);before=state;assert(!taxi_passenger_init_flag(&state,&r));
  for(unsigned i=0;i<44;i++){int changed=0;for(unsigned j=0;j<sizeof written/sizeof *written;j++)if(i==written[j])changed=1;if(!changed)assert(state.bytes[i]==before.bytes[i]);}
  assert(state.bytes[0]==1&&state.bytes[1]==0&&state.bytes[16]==0xda&&state.bytes[17]==0x56&&state.bytes[18]==1&&state.bytes[19]==0);
  assert(state.bytes[20]==0x39&&state.bytes[21]==0x30&&state.bytes[22]==0&&state.bytes[23]==0);
  assert(state.bytes[24]==0xfd&&state.bytes[25]==0xff&&state.bytes[26]==0xff&&state.bytes[27]==0xff);
  assert(state.bytes[28]==0x7c&&state.bytes[29]==0xc4&&state.bytes[30]==0x7e&&state.bytes[31]==8);
  assert(state.bytes[32]==32&&state.bytes[33]==64&&state.bytes[34]==5&&state.bytes[35]==6);
 }
 before=state;r.kind=10;assert(taxi_passenger_init_flag(&state,&r)==-1&&!memcmp(&before,&state,sizeof state));assert(taxi_passenger_init_flag(0,&r)==-1);
 puts("PASS Crazy Flag actor initialization: ROM X/Z, caller ground Y, source fields, all256 retained-byte patterns, atomic invalid kind.");return 0;
}
