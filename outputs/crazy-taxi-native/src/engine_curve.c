#include "engine_curve.h"
#include <string.h>
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
static int32_t mul(int32_t a,int32_t b) {return bits((uint32_t)a*(uint32_t)b);}
static int32_t sub(int32_t a,int32_t b) {return bits((uint32_t)a-(uint32_t)b);}
int taxi_engine_curve_sample(const int32_t curve[16],int16_t rpm,int8_t gear,
    int16_t upshift_rpm,int32_t scale,uint32_t *output) {
    if(!curve||!output) return -1;
    int32_t bucket=gear==1?((int32_t)upshift_rpm-1000)/1000:rpm/1000;
    if(bucket<0||bucket>=15) return -1;
    int32_t fraction=mul(sub(rpm,mul(bucket,1000)),4096)/1000;
    int32_t change=mul(sub(curve[bucket+1],curve[bucket]),fraction);
    change=bits(((uint32_t)change>>12)|(change<0?0xfff00000u:0));
    int32_t sample=bits((uint32_t)curve[bucket]+(uint32_t)change);
    *output=(uint32_t)mul(sample,scale)>>12;return 0;
}
