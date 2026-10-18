#ifndef TAXI_PLACEMENT_REQUESTS_H
#define TAXI_PLACEMENT_REQUESTS_H
#include <stdint.h>
/* Normal mode only original15956..1599A. Positions are already projected
   caller-owned halves; source scale/angle truncate to signed16 bits. */
typedef struct {uint32_t frame,group,position[3],priority,bucket,flags,palette;int32_t scale,angle;} TaxiPlacementRequest;
void taxi_placement_requests(const uint32_t left[3],const uint32_t right[3],uint32_t scale,uint16_t roll,TaxiPlacementRequest out[2]);
#endif
