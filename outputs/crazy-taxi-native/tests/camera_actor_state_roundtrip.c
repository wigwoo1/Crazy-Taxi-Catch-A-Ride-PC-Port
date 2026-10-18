#include "../src/vehicle_start_world.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){for(unsigned flags=0;flags<256;flags++){TaxiVehicleStartPacked packed;memset(&packed,0xa5,sizeof packed);TaxiWorldState before={0},after={0};before.camera_flags_29=(uint8_t)flags;assert(!taxi_vehicle_start_world_import(&packed,&before));assert(packed.actor[0x29]==flags);assert(packed.actor[0x2a]==0xa5);assert(!taxi_vehicle_start_world_export(&after,&packed));assert(after.camera_flags_29==flags);}puts("PASS all256 actor29 packed/native roundtrips and neighboring byte preservation.");return 0;}
