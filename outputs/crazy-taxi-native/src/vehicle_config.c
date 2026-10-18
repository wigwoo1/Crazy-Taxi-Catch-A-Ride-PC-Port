#include "vehicle_config.h"
#include <string.h>
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
static int32_t mul(int32_t a,int32_t b) {return bits((uint32_t)a*(uint32_t)b);}
static int32_t add(int32_t a,int32_t b) {return bits((uint32_t)a+(uint32_t)b);}
static int32_t asr(int32_t a,unsigned n) {return bits(((uint32_t)a>>n)|(a<0?(UINT32_MAX<<(32-n)):0));}
static int32_t word(const uint8_t *b,unsigned offset) {
    b+=offset-0x4ac;return bits(b[0]|((uint32_t)b[1]<<8)|((uint32_t)b[2]<<16)|((uint32_t)b[3]<<24));
}
static int16_t half(const uint8_t *b,unsigned offset) {
    b+=offset-0x4ac;uint16_t u=(uint16_t)(b[0]|((uint16_t)b[1]<<8));int16_t s;memcpy(&s,&u,2);return s;
}
static void scales(const uint8_t *b,unsigned offset,int32_t *circumference,int32_t *radius) {
    int32_t diameter=add(mul(mul(word(b,offset),word(b,offset+4))/100,2),asr(mul(word(b,offset+8),0x1965e),12));
    *radius=mul(diameter,4096)/2000;*circumference=mul(diameter,0x3243)/1000;
}
int taxi_vehicle_config_decode(TaxiVehicleConfig *out,const uint8_t *b,size_t size) {
    if(!out||!b||size<0x248) return -1;
    TaxiVehicleConfig v={0};
    /* The 584-byte copy overwrites the earlier spawn heading654. */
    v.camera_pitch_650=word(b,0x650);v.camera_heading_654=word(b,0x654);
    v.camera_base_680=word(b,0x680);
    v.camera_rear_658=word(b,0x658);v.camera_offset_690=word(b,0x690);
    v.camera_mode3_base_660=word(b,0x660);v.camera_mode3_offset_670=word(b,0x670);
    v.flags_4ac=b[0]&~32u;v.gear_count=word(b,0x4b4);v.final_ratio=word(b,0x4d8);
    for(unsigned i=0;i<7;i++) v.ratios[i]=word(b,0x4b8+4*i);
    v.rpm_min=half(b,0x530);v.rpm_max=half(b,0x532);
    v.limit_528=word(b,0x528);v.dimension_594=word(b,0x594);
    v.capacity_570=word(b,0x570);v.capacity_574=word(b,0x574);
    scales(b,0x5ac,&v.scale_5b8,&v.scale_5bc);scales(b,0x5c0,&v.scale_5cc,&v.scale_5d0);
    if(taxi_vehicle_speed_divisor(word(b,0x548),v.flags_4ac,v.gear_count,v.ratios,v.final_ratio,v.rpm_max,v.scale_5b8,v.scale_5cc,&v.speed_divisor_548))return -1;
    v.scale_590=mul(mul(word(b,0x584),word(b,0x588))/1000,4096)/1000;
    v.state_564=1;v.peak_6a4=word(b,0x6a4);int32_t maximum=0;
    for(unsigned i=0;i<16;i++) {
        v.curve[i]=word(b,0x4e8+4*i);
        if(v.curve[i]>=maximum) {maximum=v.curve[i];v.peak_6a4=(int32_t)i*1000;}
    }
    *out=v;return 0;
}

int taxi_vehicle_speed_divisor(int32_t initial,uint8_t flags,int32_t count,
 const int32_t ratios[7],int32_t final_ratio,int16_t rpm_max,
 int32_t wheel_5b8,int32_t wheel_5cc,int32_t *out){
 if(!out)return -1;
 if(initial){*out=initial;return 0;}
 if(!ratios||count<1||count>7)return -1;
 int32_t product=mul(ratios[count-1],final_ratio);unsigned mode=(flags>>2)&3;
 if(mode>1){*out=0;return 0;}
 int32_t denominator=asr(product,16);if(!denominator)return -1;
 int32_t numerator=bits((uint32_t)(mul(mode?wheel_5b8:wheel_5cc,rpm_max)/60)<<8);
 if(numerator==INT32_MIN&&denominator==-1)return -1;
 *out=numerator/denominator;return 0;
}
