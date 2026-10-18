#include "../src/passenger_idle.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void put(uint8_t *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(i*8));}
int main(void){
    TaxiPassengerState p={0};TaxiPickupState pickup={0};TaxiRoutePoint taxi={0};
    p.bytes[0]=65;p.bytes[10]=1;p.bytes[9]=10;p.bytes[12]=10;put(p.bytes+28,0x087ec484);
    for(int i=0;i<89;i++)assert(!taxi_passenger_idle(&p,0,&pickup,taxi,1,0,0,120));
    assert(p.bytes[11]==89&&p.bytes[0]==65&&p.bytes[10]==1);
    assert(!taxi_passenger_idle(&p,0,&pickup,taxi,1,0,0,120));
    assert(p.bytes[11]==90&&p.bytes[0]==65); /* Counter alone does not hide. */
    for(int i=90;i<130;i++)assert(!taxi_passenger_idle(&p,0,&pickup,taxi,1,0,0,120));
    assert(p.bytes[0]==0&&p.bytes[10]==0&&p.bytes[12]==120&&p.bytes[8]==0);
    memset(&p,0,sizeof(p));p.bytes[0]=65;p.bytes[10]=1;p.bytes[9]=10;p.bytes[12]=10;put(p.bytes+28,0x087ec484);
    assert(!taxi_passenger_idle(&p,0,&pickup,taxi,1,1,0,120));
    assert(p.bytes[11]==1&&p.bytes[12]==10); /* Fade is before freeze. */
    taxi.x=12800;assert(!taxi_passenger_idle(&p,0,&pickup,taxi,1,0,0,120));assert(p.bytes[11]==1);
    taxi.x=0;p.bytes[11]=127;assert(!taxi_passenger_idle(&p,0,&pickup,taxi,1,1,0,120));assert(p.bytes[11]==128);
    memset(&p,0,sizeof(p));p.bytes[0]=1;
    assert(!taxi_passenger_idle(&p,0,&pickup,taxi,1,0,1,599));assert(p.bytes[10]==2&&p.bytes[12]==10);
    for(int i=0;i<130;i++)assert(!taxi_passenger_idle(&p,0,&pickup,taxi,1,0,0,599));
    assert(p.bytes[0]==1&&p.bytes[10]==0&&p.bytes[12]==87&&p.bytes[13]==2);
    memset(&p,0,sizeof(p));p.bytes[0]=1;p.bytes[12]=2;
    uint32_t rng=0;
    assert(!taxi_passenger_idle_rng(&p,0,&pickup,taxi,1,1,&rng)&&rng==0);
    pickup.pending_1704=1;
    assert(!taxi_passenger_idle_rng(&p,1,&pickup,taxi,1,0,&rng)&&rng==0);
    pickup.pending_1704=0;
    assert(!taxi_passenger_idle_rng(&p,0,&pickup,taxi,1,0,&rng)&&rng==0);
    assert(!taxi_passenger_idle_rng(&p,0,&pickup,taxi,1,0,&rng));
    assert(rng==0x1723a9dd&&p.bytes[10]==2);
    for(int i=0;i<129;i++)assert(!taxi_passenger_idle_rng(&p,0,&pickup,taxi,1,0,&rng));
    assert(rng==0x1723a9dd);
    assert(!taxi_passenger_idle_rng(&p,0,&pickup,taxi,1,0,&rng));
    assert(rng==0x284753ba && p.bytes[10]==0);
    unsigned delay=p.bytes[12]|((unsigned)p.bytes[13]<<8);
    assert(delay==120+0x284753ba%481);
    puts("Native passenger gestures: source frames, fade and conditional original RNG draws passed");return 0;
}
