#ifdef NDEBUG
#undef NDEBUG
#endif
#include "render_interpolation.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
int main(void){
    TaxiPoint a={INT_MIN,-100,100},b={INT_MAX,100,-100};
    TaxiPoint p=taxi_render_point(a,b,.5);assert(p.x==0&&p.y==0&&p.z==0);
    assert(taxi_render_point(a,b,0).x==INT_MIN&&taxi_render_point(a,b,1).x==INT_MAX);
    assert(taxi_render_point(a,b,NAN).x==INT_MIN);
    for(unsigned phase=0;phase<4096;phase++){
        int32_t start=(int32_t)(phase<<12),end=(int32_t)(((phase+1)&4095)<<12);
        assert(((unsigned)taxi_render_angle(start,end,.5)&0xffffffu)==(((phase<<12)+2048)&0xffffffu));
        assert(taxi_render_angle(start,end,0)==start&&taxi_render_angle(start,end,1)==end);
        assert(((unsigned)taxi_render_angle(end,start,.5)&0xffffffu)==(((phase<<12)+2048)&0xffffffu));
    }
    assert(taxi_render_angle(-4096,4096,.5)==0);
    assert(taxi_render_angle(0,0x800000,.5)==0x400000);
    TaxiTrafficActor previous={.x=100,.y=200,.z=300,.heading=4095,.route=4};
    TaxiTrafficActor current={.x=200,.y=400,.z=600,.heading=1,.route=4,.collision_timer=30,.impulse_x=4267};
    TaxiTrafficActor rendered=taxi_render_traffic_actor(&previous,&current,.5);
    assert(rendered.x==150&&rendered.y==300&&rendered.z==450&&rendered.heading==0);
    rendered.x=current.x;rendered.y=current.y;rendered.z=current.z;rendered.heading=current.heading;
    assert(!memcmp(&rendered,&current,sizeof current));
    previous.countdown=1;rendered=taxi_render_traffic_actor(&previous,&current,.5);
    assert(!memcmp(&rendered,&current,sizeof current));
    previous.countdown=0;previous.route=3;rendered=taxi_render_traffic_actor(&previous,&current,.5);
    assert(!memcmp(&rendered,&current,sizeof current));
    puts("Native presentation interpolation: source endpoints, signed position extremes, all4096 angle wraps and shortest arcs PASS");
    return 0;
}
