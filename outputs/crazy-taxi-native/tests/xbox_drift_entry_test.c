#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include "xbox.h"
#include "steering_input.h"
int main(void){
 for(int lx=-32768;lx<=32767;lx++)for(unsigned phase=0;phase<3;phase++){
  int16_t original=taxi_xbox_steering_command((int16_t)lx);
  assert(taxi_xbox_vehicle_steering_command((int16_t)lx,(uint8_t)phase,0)==original);
  assert(taxi_xbox_vehicle_steering_command((int16_t)lx,(uint8_t)phase,45)==(phase==1&&original?0:original));
 }
 for(unsigned right=0;right<2;right++){
  TaxiXboxSample pad=taxi_xbox_decode(0,right?32767:-32768,0,255,0);unsigned held=pad.active_low_keys^1023;
  TaxiCommand command={0};command.bytes[4]=(uint8_t)(4|((held&32)?1:0)|((held&16)?2:0));
  int16_t analog=taxi_xbox_vehicle_steering_command(pad.steering,1,45);command.bytes[12]=(uint8_t)analog;command.bytes[13]=(uint8_t)((uint16_t)analog>>8);
  TaxiGearState car={.phase_e6a=1};taxi_steering_input(&car,&command,96,1,0,45);
  assert(car.phase_e6a==2&&car.phase_e6b==0&&car.steering_578==(right?-96:96));
  assert(taxi_xbox_vehicle_steering_command(pad.steering,2,45)==(right?-192:192));
 }
 puts("PASS Xbox drift-entry adapter:196608 stick/phase cases preserve normal analog; pending native transition uses decoded digital direction and original steering routine reachesphase2 bothdirections. Synthetic XInput, no physical controller claim.");return 0;
}
