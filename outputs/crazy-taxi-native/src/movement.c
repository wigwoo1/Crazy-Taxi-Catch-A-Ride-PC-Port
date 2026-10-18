#include "movement.h"
#include <limits.h>
#include <string.h>
static int32_t bits(uint32_t u) { int32_t s;memcpy(&s,&u,4);return s; }
static int32_t add(int32_t a,int32_t b) { return bits((uint32_t)a+(uint32_t)b); }
static int32_t sub(int32_t a,int32_t b) { return bits((uint32_t)a-(uint32_t)b); }
static int32_t mul(int32_t a,int32_t b) { return bits((uint32_t)a*(uint32_t)b); }
static int32_t asr(int32_t a,unsigned n) { return bits(((uint32_t)a>>n)|(a<0?(UINT32_MAX<<(32-n)):0)); }
static int32_t div32(int32_t a,int32_t b) { return b?bits((uint32_t)((int64_t)a/b)):a; }
/* Integer sqrt replaces BIOS SWI 08; input includes original wrapped sums. */
static int32_t root(uint32_t n) {
    uint32_t r=0,b=UINT32_C(1)<<30;
    while(b>n) b>>=2;
    while(b) { if(n>=r+b) {n-=r+b;r=(r>>1)+b;}else r>>=1;b>>=2; }
    return (int32_t)r;
}
static int move_position_impl(TaxiPoint start,TaxiPoint d,int preserve_y,
    TaxiObstacleQuery query,void *context,TaxiMovementResult *out) {
    if(!out || !query) return -1;
    *out=(TaxiMovementResult){.position=start};
    unsigned steps=(unsigned)root((uint32_t)add(add(mul(d.x,d.x),mul(d.y,d.y)),mul(d.z,d.z)))/256+1;
    TaxiPoint step={d.x/(int32_t)steps,d.y/(int32_t)steps,d.z/(int32_t)steps};
    TaxiPoint p={add(start.x,step.x),start.y,add(start.z,step.z)};
    for(unsigned j=0;j<steps;j++) {
        TaxiObstacleHit hit={.height=INT32_MIN};out->queries++;
        int kind=query(context,p,&hit);
        if(kind<0 || kind>2) return -1;
        if(kind==2) {
            if(hit.edge_count<3 || hit.edge_count>4) return -1;
            unsigned selected=0;
            int32_t lowest=asr(add(hit.vertices[0].y,hit.vertices[1].y),1);
            for(unsigned k=1;k<hit.edge_count;k++) {
                int32_t y=asr(add(hit.vertices[k].y,hit.vertices[(k+1)%hit.edge_count].y),1);
                if(y<lowest) {lowest=y;selected=k;}
            }
            TaxiPoint a=hit.vertices[selected],b=hit.vertices[(selected+1)%hit.edge_count];
            int32_t nx=sub(a.z,b.z),nz=sub(b.x,a.x);
            int32_t length=root((uint32_t)add(mul(nx,nx),mul(nz,nz)));
            nx=div32(mul(nx,256),length);nz=div32(mul(nz,256),length);
            int32_t dist=asr(add(mul(sub(a.x,p.x),nx),mul(sub(a.z,p.z),nz)),8);
            out->position.x=add(add(p.x,asr(mul(nx,dist),8)),asr(mul(nx,64),8));
            out->position.z=add(add(p.z,asr(mul(nz,dist),8)),asr(mul(nz,64),8));
            out->normal=(TaxiPoint){nx,0,nz};out->collided=1;return 0;
        }
        if(kind==0 || hit.height==INT32_MIN || sub(hit.height,p.y)<655) {
            out->position.x=add(out->position.x,step.x);
            out->position.z=add(out->position.z,step.z);
            p.x=add(p.x,step.x);p.z=add(p.z,step.z);
            if(!preserve_y) p.y=hit.height;
        }else {
            int32_t length=root((uint32_t)add(mul(d.x,d.x),mul(d.z,d.z)));
            out->normal=(TaxiPoint){div32(mul(sub(0,d.x),256),length),0,
                                   div32(mul(sub(0,d.z),256),length)};
            out->collided=1;return 0;
        }
    }
    return 0;
}
static TaxiMovementObserver movement_observer;
void taxi_movement_observer(TaxiMovementObserver value){movement_observer=value;}
int taxi_move_position(TaxiPoint start,TaxiPoint displacement,int preserve_y,TaxiObstacleQuery query,void *context,TaxiMovementResult *result){
 int rc=move_position_impl(start,displacement,preserve_y,query,context,result);
 if(!rc&&movement_observer)movement_observer(start,displacement,preserve_y,query,context,result);
 return rc;
}
int taxi_move_vehicle(TaxiVehicleBody *body,TaxiPoint displacement,uint8_t settings,
    uint8_t submode,const TaxiTrig *trig,TaxiObstacleQuery query,void *context,
    TaxiMovementResult *result,TaxiImpactEffects *effects) {
    if(!body || !trig || !result || !effects) return -1;
    *effects=(TaxiImpactEffects){0};
    int preserve=(settings&0xe0)==0x80 && submode==5;
    if(taxi_move_position(body->position,displacement,preserve,query,context,result)<0) return -1;
    body->position=result->position;
    if(result->collided) {
        body->state_e84=4096;
        taxi_vehicle_impact(&body->impact,result->normal,settings,submode,trig,effects);
    }
    return 0;
}
