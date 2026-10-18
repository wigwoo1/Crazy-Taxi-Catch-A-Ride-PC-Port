#include "traction.h"
#include <string.h>
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
static int32_t mul(int32_t a,int32_t b) {return bits((uint32_t)a*(uint32_t)b);}
static int32_t neg(int32_t a) {return bits(0u-(uint32_t)a);}
static int32_t asr(int32_t a,unsigned n) {return bits(((uint32_t)a>>n)|(a<0?(UINT32_MAX<<(32-n)):0));}
static uint32_t root(uint32_t n) {
    uint32_t result=0,bit=1u<<30;
    while(bit>n) bit>>=2;
    while(bit) {if(n>=result+bit) {n-=result+bit;result=(result>>1)+bit;} else result>>=1;bit>>=2;}
    return result;
}
static uint32_t length(int32_t a,int32_t b) {return root((uint32_t)mul(a,a)+(uint32_t)mul(b,b))&65535;}
void taxi_angle_table_decode(TaxiAngleTable *t,const uint8_t b[514]) {
    for(unsigned i=0;i<257;i++) {uint16_t u=(uint16_t)(b[2*i]|((uint16_t)b[2*i+1]<<8));memcpy(&t->values[i],&u,2);}
}
int taxi_vector_angle(int32_t x,int32_t z,const TaxiAngleTable *t,int32_t *out) {
    int32_t a=x<0?neg(x):x,b=z<0?neg(z):z;
    int flip=b<=a;int32_t denominator=flip?a:b;
    if(!denominator) denominator=1;
    int32_t numerator=mul(flip?b:a,256);
    int64_t index=(int64_t)numerator/denominator;
    if(index<0||index>256) return -1;
    int32_t angle=t->values[index];
    if(flip) angle=1024-angle;
    if(x>=0) angle=z<0?2048-angle:angle&4095;
    else angle=z<0?angle+2048:4096-angle;
    *out=angle;return 0;
}
static int traction_step_impl(TaxiTraction *v,TaxiTractionContext *c,const TaxiTrig *t,
    const TaxiAngleTable *table,TaxiOrientationEffects *effects) {
    int32_t angle;
    if(taxi_vector_angle(neg(v->force_53c),v->force_540,table,&angle)) return -1;
    int32_t a,b;
    if(v->phase_e6a==2&&!c->flag_1696&&!c->flag_1697) {
        int divisor=((c->settings&224)!=128&&(v->phase_e6b<=119||v->speed_544<=34139))?32:8;
        a=v->capacity_574/divisor;b=v->capacity_570/divisor;c->flag_25d4=1;
    } else {
        a=mul(v->capacity_574,6144)/4096;b=mul(v->capacity_570,6144)/4096;
        if(!c->state_2630) c->flag_25d4=0;
    }
    unsigned index=(uint32_t)angle&4095;
    int32_t x=asr(mul(mul(t->cosine[index],16),b),12);
    int32_t z=asr(mul(mul(t->sine[index],16),a),12);
    memset(effects,0,sizeof *effects);
    if(length(neg(v->force_53c),v->force_540)>length(x,z)) {
        v->state_564=0;
        if(!c->flag_262c) {
            c->flag_262c=1;effects->score_requested=1;
            if(v->speed_544<=45520) effects->score_c=25;
            else if(v->speed_544<=91040) effects->score_c=50;
            else if(v->speed_544<=136560) effects->score_c=75;
            else effects->score_b=1;
        }
    } else {v->state_564=1;c->flag_262c=0;}
    if(v->flags_28&8) v->speed_544=asr(mul(v->speed_544,4075),12);
    return 0;
}

static TaxiTractionObserver observer;
void taxi_traction_observer(TaxiTractionObserver value){observer=value;}
int taxi_traction_step(TaxiTraction *v,TaxiTractionContext *c,const TaxiTrig *trig,const TaxiAngleTable *table,TaxiOrientationEffects *effects){
 if(!observer)return traction_step_impl(v,c,trig,table,effects);
 TaxiTraction before=*v;TaxiTractionContext context=*c;int rc=traction_step_impl(v,c,trig,table,effects);
 if(!rc)observer(&before,&context,v,c,effects);return rc;
}
