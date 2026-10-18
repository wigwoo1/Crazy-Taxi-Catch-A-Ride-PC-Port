#include "traffic_presentation.h"
#include "traffic_heading.h"
#include <string.h>
static int32_t bits(uint32_t x){int32_t s;memcpy(&s,&x,4);return s;}
static int32_t magnitude(int32_t x){return x<0?bits(0u-(uint32_t)x):x;}
int taxi_traffic_presentation_prepare(TaxiTrafficActor *a,int32_t cx,int32_t cz,
    int32_t delta,int paused,TaxiTrafficHeightQuery height,void *context){
    if(!a||!height)return -1;
    if(a->countdown>0)return 0;
    int32_t dx=bits((uint32_t)a->x-(uint32_t)cx),dz=bits((uint32_t)a->z-(uint32_t)cz);
    if(magnitude(dx)>23040||magnitude(dz)>23040){a->countdown=180;return 0;}
    TaxiTrafficActor next=*a;
    if(height(context,(TaxiPoint){next.x,next.y,next.z},&next.y)<0)return -1;
    if(!next.collision_timer&&!paused){
        next.heading=bits(taxi_traffic_heading_approach((uint32_t)next.heading,
            0u-(uint32_t)next.path.heading,bits((uint32_t)delta<<4)));
    }
    *a=next;return 1;
}
