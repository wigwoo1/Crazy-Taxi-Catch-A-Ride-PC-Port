#include "vehicle_start_packed.h"
#include <assert.h>
#include <stdio.h>
int main(int argc,char **argv){assert(argc==2);FILE *f=fopen(argv[1],"rb");assert(f);unsigned seed,settings,demo,hit;
 while(scanf("%u %u %u %u",&seed,&settings,&demo,&hit)==4){uint8_t data[584];assert(!fseek(f,demo?0x4a5794:0x4a554c,SEEK_SET));assert(fread(data,1,584,f)==584);TaxiVehicleStartPacked s;
 for(unsigned i=0;i<sizeof s.actor;i++)s.actor[i]=(uint8_t)(seed+i*37);s.rng=seed;s.state_2630=12345;s.last_sprite_53e0=12345;
 TaxiVehicleStartPoint point={100,200,1000,3073,17,29,33,(uint8_t)hit};assert(taxi_vehicle_start_packed(&s,&point,data,sizeof data,(uint8_t)settings,(uint8_t)demo)==(int)hit);
 for(unsigned i=0;i<sizeof s.actor;i++)printf("%02x",s.actor[i]);printf(" %d %d %u\n",s.state_2630,s.last_sprite_53e0,s.rng);
 }fclose(f);return 0;}
