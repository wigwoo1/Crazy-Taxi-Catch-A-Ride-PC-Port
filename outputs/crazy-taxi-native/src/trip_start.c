#include "trip_start.h"
#include <string.h>
static int32_t bits(uint32_t u) { int32_t s; memcpy(&s,&u,4); return s; }
static int32_t mul(int32_t a,int32_t b) { return bits((uint32_t)a*(uint32_t)b); }
static int32_t asr(int32_t a,unsigned n) {
    return bits(((uint32_t)a>>n)|(a<0?~(UINT32_MAX>>n):0));
}
/* ROM 084a5054, 084a5068, 084a507c; scalar 084a59dc = 60. */
static const int32_t allowance[5]={2048,2304,2560,2816,3072};
static const int32_t category_scale[5]={4096,3968,3840,3712,3584};
static const int32_t difficulty[8]={26880,26453,26026,25600,24640,23680,22720,21760};
int taxi_trip_start(TaxiTripStartState *state,const TaxiTripStartInput *in,uint8_t *palette) {
    if(!state||!in||!palette||in->customer_index<0||in->customer_index>=128) return -1;
    unsigned mode=in->settings_0&0xe0, setting=in->settings_3&15;
    if(mode!=0x80&&(in->category>=5||setting<1||setting>8)) return -1;
    TaxiTripStartState s=*state;
    s.scale_5310=s.scale_5312=256;s.phase_5314=0;
    s.active_customer=in->customer_index;s.direction_753c=-1;
    s.active_1694=1;s.voice_25d6=(uint8_t)(in->random_result%2+12);
    if(mode!=0x80) {
        int32_t t=mul((in->route_length/256)/16,60);
        t=asr(mul(t,category_scale[in->category]),12);
        t=asr(mul(t,difficulty[setting-1])/100,8);
        if(mode==0) {
            s.allowance_16e0=asr(mul(t,allowance[in->category]),12);
            int32_t cap=bits((uint32_t)s.allowance_16e0+(uint32_t)in->remaining_16a8);
            if(t>cap) t=cap;
        }
        s.time_16ac=s.initial_time_16b0=t;s.active_1695=1;
    }
    *state=s;*palette=(uint8_t)(mode!=0x80);return 0;
}
