/* Diagnostic wrapper; includes current production world code verbatim. */
#include "../src/drive_world.h"
int capture_full_driver(TaxiDriver*,TaxiDriverContext*,TaxiCommand*,const TaxiGearInput*,uint8_t,uint8_t,const TaxiTrig*,const TaxiAngleTable*,TaxiDriverEffects*);
#define taxi_driver_step capture_full_driver
#include "../src/drive_world.c"
