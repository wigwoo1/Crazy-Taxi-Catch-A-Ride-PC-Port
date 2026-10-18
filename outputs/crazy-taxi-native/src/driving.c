#include "driving.h"
#include <string.h>
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
static int32_t add(int32_t a,int32_t b) {return bits((uint32_t)a+(uint32_t)b);}
static int32_t sub(int32_t a,int32_t b) {return bits((uint32_t)a-(uint32_t)b);}
static int32_t mul(int32_t a,int32_t b) {return bits((uint32_t)a*(uint32_t)b);}
static int32_t asr(int32_t a,unsigned n) {return bits(((uint32_t)a>>n)|(a<0?(UINT32_MAX<<(32-n)):0));}
void taxi_command_pack(TaxiCommand *c,const TaxiInput *in) {
    c->bytes[4]=(uint8_t)((in->held[5]&1)|((in->held[4]&1)<<1)
        |((in->held[0]&1)<<2)|((in->held[1]&1)<<3)|((in->held[8]&1)<<4)
        |((in->held[9]&1)<<5)|((in->held[5]&1)<<6)|((in->held[4]&1)<<7));
    c->bytes[5]&=252;
    memset(c->bytes+6,0,10);
}
int taxi_steering_response(TaxiSteeringState *v,const TaxiCommand *command,
    uint8_t settings,const TaxiTrig *trig,int32_t *response) {
    /* Sixteen consecutive words at 084a59f4; the final entry is used as
       the interpolation endpoint, not a valid starting bucket. */
    static const int32_t curve[16]={682,398,227,170,113,68,45,34,34,34,34,34,34,34,34,34};
    if(!v || !command || !trig || !response) return -1;
    int32_t speed=v->speed_544<0?sub(0,v->speed_544):v->speed_544;
    int32_t scaled=speed/1138,bucket=scaled/25;
    if(bucket<0 || bucket>=15) return -1;
    int32_t factor;
    if(v->phase_e6a==2 && !(v->flags_28&8)) factor=(settings&0xe0)==0x80?8192:12288;
    else {
        factor=5570;
        if(bucket!=0) factor-=82;
        if(bucket>2) factor-=82;
        if(bucket>4) factor-=82;
        if(bucket>6) factor-=164;
        if(bucket>6 && bucket<=8) factor+=82;
    }
    int32_t low=asr(mul(curve[bucket],factor),12);
    int32_t high=asr(mul(curve[bucket+1],factor),12);
    int32_t fraction=mul(sub(scaled,mul(bucket,25)),4096)/25;
    int32_t result=add(low,asr(mul(fraction,sub(high,low)),12));
    uint8_t buttons=command->bytes[4];
    if((buttons&8) || !(buttons&4)) {
        v->flags_28=(uint8_t)((v->flags_28|32)&~64);
        factor=v->state_564?2048:2662;
    }else if(v->state_564) {
        v->flags_28&=(uint8_t)~96;
        factor=2048;
    }else {
        if(v->phase_e6a==2) v->flags_28=(uint8_t)((v->flags_28|32)&~64);
        else v->flags_28=(uint8_t)((v->flags_28|64)&~32);
        if(v->phase_e6a==2 && v->phase_e6b<=119 && v->speed_544<=34139)
            factor=sub(7372,mul(5529,v->speed_544)/34140);
        else factor=1843;
    }
    if(!(v->flags_358&4)) factor=asr(mul(4710,factor),12);
    result=asr(mul(result,factor),12);
    if(!(v->flags_358&4)) {
        if(v->limit_7b0<result) {result=v->limit_7b0;v->state_7b8=0;}
        else v->state_7b8=1;
    }
    result=mul(mul(trig->sine[v->angle_578&4095],16),result);
    *response=(v->flags_28&8)?result/16384:asr(result,12);
    return 0;
}
