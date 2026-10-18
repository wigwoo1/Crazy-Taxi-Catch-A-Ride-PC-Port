#ifndef TAXI_TRAFFIC_HEADING_H
#define TAXI_TRAFFIC_HEADING_H
#include <stdint.h>
/* Original 34064 veneer -> ARM 531c. Result retains unmasked current/target. */
uint32_t taxi_traffic_heading_approach(uint32_t current,uint32_t target,int32_t step);
#endif
