#ifndef TAXI_ENGINE_CURVE_H
#define TAXI_ENGINE_CURVE_H
#include <stdint.h>
/* Scalar throttle-path block 08013a38..08013ab2 only. The caller must apply
   action/shift/rev-limit gates. This is not acceleration or a speed update.
   Gear 1 selects its segment from upshift_rpm-1000 but still interpolates
   with current RPM. Final wrapped product uses a logical right shift.
   Invalid adjacent-table reads return -1 without changing output. */
int taxi_engine_curve_sample(const int32_t curve[16],int16_t rpm,int8_t gear,
    int16_t upshift_rpm,int32_t scale_4b0,uint32_t *output);
#endif
