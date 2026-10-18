#ifndef TAXI_VEHICLE_CONFIG_H
#define TAXI_VEHICLE_CONFIG_H
#include <stddef.h>
#include <stdint.h>
typedef struct {
    uint8_t flags_4ac;
    int32_t camera_pitch_650,camera_heading_654,camera_base_680;
    /* Legacy field658 retained; original rear-position uses690. */
    int32_t camera_offset_690;
    int32_t camera_rear_658,camera_mode3_base_660,camera_mode3_offset_670;
    int32_t gear_count,ratios[7],final_ratio,curve[16];
    int16_t rpm_min,rpm_max;
    int32_t speed_divisor_548,limit_528,scale_590,dimension_594,capacity_570,capacity_574;
    int32_t scale_5b8,scale_5bc,scale_5cc,scale_5d0,peak_6a4,state_564;
} TaxiVehicleConfig;
/* Native configuration calculations from 08011804 through 08011a1c.
   Takes the 0x248-byte template at 084a554c or 084a5794. Does not execute
   the subsequent reset helper 08011540 or initialize a complete vehicle.
   Returns -1 on short data, invalid used gear index or zero used divisor;
   output is unchanged on error. Offsets remain names where meaning is unknown. */
int taxi_vehicle_config_decode(TaxiVehicleConfig *out,const uint8_t *bytes,size_t size);
/* Original11908..11998; retain supplied nonzero divisor, derive mode0/1.
   Signed divisions and low-word products match source; invalid used divisor
   or gear index rejects without changing output. */
int taxi_vehicle_speed_divisor(int32_t initial,uint8_t flags,int32_t count,
 const int32_t ratios[7],int32_t final_ratio,int16_t rpm_max,
 int32_t wheel_5b8,int32_t wheel_5cc,int32_t *out);
#endif
