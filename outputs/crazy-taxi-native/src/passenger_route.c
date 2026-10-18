#include "passenger_route.h"
#include <string.h>
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
static int32_t sub(int32_t a,int32_t b) {return bits((uint32_t)a-(uint32_t)b);}
static int32_t asr8(int32_t a) {return bits(((uint32_t)a>>8)|(a<0?0xff000000u:0));}
static uint32_t root(uint32_t n) {
    uint32_t result=0,bit=1u<<30;
    while(bit>n) bit>>=2;
    while(bit) {if(n>=result+bit) {n-=result+bit;result=(result>>1)+bit;} else result>>=1;bit>>=2;}
    return result;
}
/* 08032488 signed divide: zero divisor calls empty0803251c then returns0.
   Widening also avoids host UB on INT_MIN/-1; retain original low32bits. */
static int32_t divide(int32_t a,int32_t b) {return b?bits((uint32_t)((int64_t)a/b)):0;}
int taxi_passenger_route_length(const TaxiRoutePoint *points,size_t count,int32_t *length) {
    if(!points||!count||!length) return -1;
    uint32_t sum=0;
    for(size_t i=1;i<count;i++) {
        uint32_t x=(uint32_t)asr8(sub(points[i].x,points[i-1].x));
        uint32_t z=(uint32_t)asr8(sub(points[i].z,points[i-1].z));
        sum+=root(x*x+z*z)&65535u;
    }
    *length=bits(sum<<8);return 0;
}
TaxiRouteCategories taxi_passenger_route_categories(int32_t length,int32_t minimum,
    int32_t maximum,TaxiRoutePoint origin,TaxiRoutePoint destination) {
    int32_t width=divide(sub(maximum,minimum),5);
    int32_t category=divide(sub(length,minimum),width);
    if(category>4) category=4;
    uint32_t x=(uint32_t)sub(destination.x,origin.x),z=(uint32_t)sub(destination.z,origin.z);
    uint32_t distance=root(x*x+z*z)&65535u;
    int32_t ratio=divide(bits(distance*100u),length);
    return (TaxiRouteCategories){(uint8_t)category,
        (uint8_t)(ratio<20?4:ratio<40?3:ratio<60?2:ratio<80?1:0)};
}
