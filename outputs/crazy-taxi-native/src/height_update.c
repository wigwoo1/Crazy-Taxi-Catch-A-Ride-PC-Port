#include "height_update.h"
#include <string.h>
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
static int32_t add(int32_t a,int32_t b) {return bits((uint32_t)a+(uint32_t)b);}
static int32_t sub(int32_t a,int32_t b) {return bits((uint32_t)a-(uint32_t)b);}
static int16_t half(int32_t a) {uint16_t u=(uint16_t)a;int16_t s;memcpy(&s,&u,2);return s;}
static uint32_t root(uint32_t n) {
    uint32_t result=0,bit=1u<<30;
    while(bit>n) bit>>=2;
    while(bit) {if(n>=result+bit) {n-=result+bit;result=(result>>1)+bit;} else result>>=1;bit>>=2;}
    return result;
}
static int32_t distance(TaxiPoint a,TaxiPoint b) {
    uint32_t x=(uint32_t)a.x-(uint32_t)b.x,y=(uint32_t)a.y-(uint32_t)b.y,z=(uint32_t)a.z-(uint32_t)b.z;
    return (int32_t)(root(x*x+y*y+z*z)&65535);
}
int taxi_height_update(TaxiHeightState *out,TaxiHeightGlobals *global_out,
    const TaxiWorldContact *c,const TaxiHeightInput *in,
    const TaxiNormalAngles *angles,TaxiHeightEffects *effects) {
    if(!out||!global_out||!c||!in||!angles||!effects||c->height==INT32_MIN||
       (!c->normal.x&&!c->normal.y&&!c->normal.z)) return -1;
    TaxiHeightState v=*out;TaxiHeightGlobals g=*global_out;TaxiHeightEffects e={0};
    int special=(in->settings&0xe0)==0x80;
    int32_t threshold=add(c->height,bits((uint32_t)in->delta<<5));
    if(v.flags_28&8) {
        if(!g.flag_153e) g.distance_1540=distance(g.takeoff_1544,v.position);
        if(v.position.y<=threshold) {
            v.flags_28&=(uint8_t)~8;v.position.y=c->height;e.landed=1;
            v.body.position_734=v.body.response_704;
            v.body.position_740=v.body.response_70c;v.state_698=0;
            if(!g.flag_153e) g.distance_1540=distance(g.takeoff_1544,v.position);
            if(special&&!in->submode&&!g.flag_153e&&v.position.x>0x8a00) {
                g.flag_153e=1;
                if(g.distance_1540>0x9600) {
                    g.flag_153c=1;e.completion_call=1;g.flag_153d=in->completion_result;
                } else g.flag_153c=0;
                g.flag_1697=1;
            }
        } else {
            if(!special) {
                int32_t gap=sub(v.position.y,c->height);
                if(gap<0) gap=sub(c->height,v.position.y);
                if(gap>2048) {e.airborne_call=1;e.airborne_arg1=1;}
                else if(gap>1536) {e.airborne_call=1;e.airborne_arg2=75;}
                else if(gap>1024) {e.airborne_call=1;e.airborne_arg2=50;}
                else if(gap>512) {e.airborne_call=1;e.airborne_arg2=25;}
            }
            v.position.y=add(v.position.y,v.displacement_y);
            v.body.response_704=add(v.body.response_704,v.body.velocity_730);
            v.body.response_70c=add(v.body.response_70c,v.body.velocity_732);
        }
    } else {
        v.state_698=0;
        if(add(v.position.y,v.displacement_y)<=threshold) v.position.y=c->height;
        else {
            if(special&&!in->submode&&v.position.x<=0x8a00) {
                v.flags_28&=(uint8_t)~8;v.position.y=c->height;
            } else v.flags_28|=8;
            if(v.flags_28&8) {
                e.took_off=1;
                if(!g.flag_153e) g.takeoff_1544=v.position;
                uint32_t factor=special&&!in->submode?(v.scale_e84==4096?3:2):
                    special&&in->submode==5?10:7;
                v.body.velocity_730=half(bits(0u-(uint32_t)in->delta*factor));
                if(!(special&&!in->submode)) {
                    int16_t cap=special&&in->submode==5?512:341;
                    if(v.body.velocity_730>cap) v.body.velocity_730=cap;
                }
                v.body.velocity_732=0;
                v.position.y=add(v.position.y,v.displacement_y);
            }
        }
    }
    if(!(v.flags_28&8)) {
        if(taxi_body_springs(&v.body)||taxi_body_align(&v.body,c->normal,in->delta,angles)) return -1;
    }
    *out=v;*global_out=g;*effects=e;return 0;
}
