#include "body_pose.h"
#include <string.h>
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
static int32_t add(int32_t a,int32_t b) {return bits((uint32_t)a+(uint32_t)b);}
static int32_t sub(int32_t a,int32_t b) {return bits((uint32_t)a-(uint32_t)b);}
static int32_t scaled(int32_t a) {return bits((uint32_t)a<<12);}
static int32_t divide(int32_t a,int32_t b) {return bits((uint32_t)((int64_t)a/b));}
static int16_t narrow(int32_t a) {uint16_t u=(uint16_t)a;int16_t s;memcpy(&s,&u,2);return s;}
int taxi_normal_angles_decode(TaxiNormalAngles *out,const uint8_t *b,size_t size) {
    if(!out||!b||size<8193*2) return -1;
    for(unsigned i=0;i<8193;i++) out->angle[i]=narrow(b[2*i]|((uint32_t)b[2*i+1]<<8));
    return 0;
}
static void spring(int32_t *position,int32_t *velocity,int32_t a,int32_t b,int clear_crossing) {
    int32_t old=*position;
    *velocity=sub(*velocity,divide(scaled(old),a));
    if(*velocity>=-1&&*velocity<=1) *velocity=0;
    *position=add(old,sub(*velocity,divide(scaled(old),b)));
    if(clear_crossing&&((old<0)!=(*position<0))) *position=*velocity=0;
}
int taxi_body_springs(TaxiBodyPose *v) {
    if(!v||!v->denominator_754||!v->denominator_758) return -1;
    spring(&v->position_734,&v->velocity_73c,v->denominator_754,v->denominator_758,1);
    v->response_704=v->position_734;
    spring(&v->position_740,&v->velocity_748,v->denominator_754,v->denominator_758,0);
    v->response_70c=v->position_740;return 0;
}
static void align_axis(int32_t *position,int16_t *velocity,int32_t target,int32_t delta) {
    int32_t change=sub(target,*position)/4;
    *velocity=narrow(add(*velocity,change));
    if(*velocity>0) {*velocity=narrow(sub(*velocity,delta));if(*velocity<0) *velocity=0;}
    else if(*velocity<0) {*velocity=narrow(add(*velocity,delta));if(*velocity>0) *velocity=0;}
    *position=add(*position,add(*velocity,change));
}
int taxi_body_align(TaxiBodyPose *v,TaxiPoint normal,int32_t delta,const TaxiNormalAngles *table) {
    if(!v||!table||normal.x<-4096||normal.x>4096||normal.z<-4096||normal.z>4096) return -1;
    align_axis(&v->orientation_6f4.x,&v->velocity_730,table->angle[normal.z+4096],delta);
    align_axis(&v->orientation_6f4.z,&v->velocity_732,-table->angle[normal.x+4096],delta);
    return 0;
}
