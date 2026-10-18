#include "taxi_placement_requests.h"
void taxi_placement_requests(const uint32_t left[3],const uint32_t right[3],uint32_t scale,uint16_t roll,TaxiPlacementRequest out[2]){
 for(unsigned i=0;i<2;i++){const uint32_t *p=i?right:left;out[i]=(TaxiPlacementRequest){i,1,{p[0],p[1],p[2]},0,2,0,0,(int16_t)(uint16_t)scale,(int16_t)(uint16_t)(0u-roll)};}
}
