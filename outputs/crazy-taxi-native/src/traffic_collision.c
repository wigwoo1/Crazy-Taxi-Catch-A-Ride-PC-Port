#include "traffic_collision.h"
#include <string.h>
static FILE *collision_trace;
void taxi_traffic_collision_trace(FILE *sink){collision_trace=sink;}
static int32_t bits(uint32_t x){int32_t s;memcpy(&s,&x,4);return s;}
static int32_t add(int32_t a,int32_t b){return bits((uint32_t)a+(uint32_t)b);}
static int32_t sub(int32_t a,int32_t b){return bits((uint32_t)a-(uint32_t)b);}
static int32_t mul(int32_t a,int32_t b){return bits((uint32_t)a*(uint32_t)b);}
static int32_t asr(int32_t a,unsigned n){return bits(((uint32_t)a>>n)|(a<0?(UINT32_MAX<<(32-n)):0));}
static uint32_t root(uint32_t n){
    uint32_t r=0,b=UINT32_C(1)<<30;while(b>n)b>>=2;
    while(b){if(n>=r+b){n-=r+b;r=(r>>1)+b;}else r>>=1;b>>=2;}return r;
}
int taxi_traffic_collision(TaxiTrafficActor *a,int32_t x,int32_t z,
    int32_t speed,int32_t sine,int32_t cosine,int32_t delta){
    if(!a)return -1;
    TaxiTrafficActor before=*a;
    int32_t sx=asr(bits(0-((uint32_t)sine<<4)),4);
    int32_t cz=asr(bits((uint32_t)cosine<<4),4);
    int32_t dx=sub(a->x,sub(x,mul(asr(mul(sx,0x10ab),8),delta)/60));
    int32_t dz=sub(a->z,sub(z,mul(asr(mul(cz,0x10ab),8),delta)/60));
    uint32_t length=root((uint32_t)add(mul(dx,dx),mul(dz,dz)))&65535;
    if(!length)return -1;
    int32_t scale=speed/0x472;if(scale>200)scale=200;
    int32_t radius=mul(192,scale)/200+384;
    a->x=add(x,bits((uint32_t)mul(dx,radius)<<1)/(int32_t)length);
    a->z=add(z,bits((uint32_t)mul(dz,radius)<<1)/(int32_t)length);
    a->collision_timer=120;
    a->impulse_x=add(a->impulse_x,mul(dx,0x10ab)/(int32_t)length);
    a->impulse_z=add(a->impulse_z,mul(dz,0x10ab)/(int32_t)length);
    a->flags&=~UINT32_C(1);
    if(collision_trace)fprintf(collision_trace,"%ld %ld %ld %ld %u %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %u %ld\n",
        (long)before.x,(long)before.z,(long)before.impulse_x,(long)before.impulse_z,before.flags,(long)before.collision_timer,
        (long)x,(long)z,(long)speed,(long)sine,(long)cosine,(long)delta,
        (long)a->x,(long)a->z,(long)a->impulse_x,(long)a->impulse_z,a->flags,(long)a->collision_timer);
    return 0;
}
static int32_t magnitude(int32_t x){return x<0?bits(0-(uint32_t)x):x;}
static int32_t damp(int32_t x){
    int32_t reduced=asr(mul(magnitude(x),243),8);
    return x<0?bits(0-(uint32_t)reduced):reduced;
}
int taxi_traffic_impulse_displacement(TaxiTrafficActor *a,int32_t steps,
    int32_t global_delta,int32_t *dx,int32_t *dz){
    if(!a||!dx||!dz)return -1;
    int32_t x=a->impulse_x,z=a->impulse_z;
    for(int32_t i=0;i<steps;i++){x=damp(x);z=damp(z);}
    if(magnitude(x)<=16&&magnitude(z)<=16)x=z=0;
    a->impulse_x=x;a->impulse_z=z;
    *dx=mul(x,global_delta)/60;*dz=mul(z,global_delta)/60;
    return 0;
}
static int32_t approach_zero(int32_t value,int32_t step){
    if(value<0){int32_t next=add(value,step);return next<=0?next:0;}
    int32_t next=sub(value,step);return next>=0?next:0;
}
int taxi_traffic_rejoin_position(TaxiTrafficActor *a,const TaxiAiPathAsset *road,
    int32_t delta,int32_t *direction_x,int32_t *direction_z){
    if(!a||!road||!direction_x||!direction_z)return -1;
    TaxiTrafficActor next=*a;
    if(taxi_traffic_path_advance(road->points,road->count,delta,&next.path))return -1;
    next.x=add(next.path.x,next.offset_x);next.z=add(next.path.z,next.offset_z);
    int32_t step=bits((uint32_t)delta<<4);
    next.offset_x=approach_zero(next.offset_x,step);
    next.offset_z=approach_zero(next.offset_z,step);
    int pending=next.offset_x||next.offset_z;
    if(!pending)next.flags|=1;
    *direction_x=sub(next.path.x,next.x);*direction_z=sub(next.path.z,next.z);
    *a=next;return pending;
}
