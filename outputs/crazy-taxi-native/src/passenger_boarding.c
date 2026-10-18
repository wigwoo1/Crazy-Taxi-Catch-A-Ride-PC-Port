#include "passenger_boarding.h"
#include <string.h>
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
static int32_t word(const uint8_t *p) {return bits((uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24));}
static void put(uint8_t *p,int32_t v) {for(unsigned i=0;i<4;i++) p[i]=(uint8_t)((uint32_t)v>>(8*i));}
static int32_t half(const uint8_t *p) {uint32_t u=(uint32_t)p[0]|((uint32_t)p[1]<<8);return u<32768?(int32_t)u:(int32_t)u-65536;}
static void put_half(uint8_t *p,int32_t v) {p[0]=(uint8_t)v;p[1]=(uint8_t)((uint32_t)v>>8);}
static int32_t signed_byte(uint8_t u) {return u<128?u:(int32_t)u-256;}
static int32_t absdiff(int32_t a,int32_t b) {int32_t v=bits((uint32_t)a-(uint32_t)b);return v<0?bits(0u-(uint32_t)v):v;}
static int32_t step(int32_t speed,int32_t delta) {int32_t v=bits((uint32_t)speed*(uint32_t)delta);return bits(((uint32_t)v>>8)|(v<0?0xff000000u:0));}
int taxi_passenger_boarding(TaxiPassengerState *out,int selected,
    const TaxiPickupState *pickup,TaxiRoutePoint taxi,int32_t delta,uint8_t freeze) {
    if(!out||!pickup) return -1;
    TaxiPassengerState v=*out;uint8_t *p=v.bytes;
    if(absdiff(taxi.x,word(p+16))>0x31ff||absdiff(taxi.z,word(p+24))>0x31ff||
       signed_byte(p[1])>7||(p[0]&3)!=1||freeze) return 0;
    int32_t stage=signed_byte(p[10]);
    if(stage>=0&&stage<=2) {
        if(!selected||!pickup->pending_1704) return 0;
        p[10]=3;put_half(p+12,5);put(p+28,0x087ec4f4);
        p[9]=5;put_half(p+14,0);p[8]=0;
    } else if(stage==3) {
        if(word(p+28)!=0x087ec4f4) return -1;
        put_half(p+12,bits((uint32_t)half(p+12)-(uint32_t)delta));
        if(half(p+12)<=0) {
            put_half(p+12,signed_byte(p[9]));
            put_half(p+14,half(p+14)+1);
            int32_t frame=half(p+14);
            if(frame<0||frame>9) return -1;
            /* Original087ec4f4: 0,1,2,3,4,5,6,7,8,-1. */
            if(frame==9) {put_half(p+14,0);frame=0;}
            p[8]=(uint8_t)frame;
        }
        int32_t x=word(p+16),z=word(p+24);
        uint32_t dx=(uint32_t)taxi.x-(uint32_t)x,dz=(uint32_t)taxi.z-(uint32_t)z;
        if(bits(dx*dx+dz*dz)<=0x3ffff) p[0]|=32;
        put(p+16,bits((uint32_t)x+(uint32_t)step(pickup->approach_x_7534,delta)));
        put(p+24,bits((uint32_t)z+(uint32_t)step(pickup->approach_z_7538,delta)));
    }
    *out=v;return 0;
}
