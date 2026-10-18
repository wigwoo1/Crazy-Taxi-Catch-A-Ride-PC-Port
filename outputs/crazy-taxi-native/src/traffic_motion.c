#include "traffic_motion.h"
#include <string.h>
static int32_t bits(uint32_t x){int32_t s;memcpy(&s,&x,4);return s;}
int taxi_traffic_postimpact_motion(TaxiTrafficActor *a,const TaxiAiPathAsset *road,
    int32_t frame_delta,int32_t global_delta,int collision,int preserve_y,
    TaxiObstacleQuery query,void *context,const TaxiAngleTable *angles){
    if(!a||!road||!query||!angles)return -1;
    TaxiTrafficActor next=*a;
    if(next.impulse_x||next.impulse_z){
        int32_t dx,dz;
        if(taxi_traffic_impulse_displacement(&next,frame_delta,global_delta,&dx,&dz))return -1;
        TaxiMovementResult result;
        if(taxi_move_position((TaxiPoint){next.x,next.y,next.z},(TaxiPoint){dx,0,dz},
            preserve_y,query,context,&result))return -1;
        next.x=result.position.x;next.z=result.position.z;
        int32_t rotation=bits((uint32_t)global_delta<<11)/60;
        next.heading=bits((uint32_t)next.heading+(uint32_t)rotation);
        next.offset_x=bits((uint32_t)next.x-(uint32_t)next.path.x);
        next.offset_z=bits((uint32_t)next.z-(uint32_t)next.path.z);
    }else if(!collision){
        int32_t dx,dz;int pending=taxi_traffic_rejoin_position(&next,road,frame_delta,&dx,&dz);
        if(pending<0)return -1;
        if(pending){int32_t angle;if(taxi_vector_angle(dx,dz,angles,&angle))return -1;
            next.path.heading=(uint16_t)angle;}
    }
    *a=next;return 0;
}
