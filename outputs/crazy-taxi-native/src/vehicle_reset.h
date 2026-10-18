#ifndef TAXI_VEHICLE_RESET_H
#define TAXI_VEHICLE_RESET_H
#include "orientation.h"
/* Fields touched by 08011540, with the existing native orientation state.
   Numeric names preserve offsets whose gameplay meaning is not established. */
typedef struct {
    TaxiOrientation orientation;
    TaxiPoint displacement;
    int32_t state_34,state_40,state_44;
    uint16_t state_2c,state_30,state_64c,state_730,state_732,state_e60;
    uint8_t state_611,state_612,state_613,state_e68,state_e69,state_e6b;
    int32_t state_614,state_618,state_61c,state_620,state_650,state_654,state_698;
    int32_t state_6f4,state_6f8,state_6fc,state_704,state_708,state_70c;
    int32_t state_734,state_738,state_73c,state_740,state_744,state_748;
    int32_t state_74c,state_750,state_754,state_758,state_75c;
    int32_t state_774,state_778,state_77c;
    int32_t state_e6c,state_e70,state_e74,state_e78,state_e7c,state_e80;
} TaxiVehicleReset;
/* Complete 08011540..080116f0 state reset. It preserves untouched orientation
   fields, including position, heading, speed, steering force and state_564.
   This helper is only one part of full vehicle initialization. */
void taxi_vehicle_reset(TaxiVehicleReset *v,uint8_t settings,int32_t *state_2630);
#endif
