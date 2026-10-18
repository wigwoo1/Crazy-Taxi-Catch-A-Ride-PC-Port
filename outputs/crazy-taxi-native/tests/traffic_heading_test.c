#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include "traffic_heading.h"
int main(void){
    const int32_t steps[]={0,1,16,2047,2048,4096,-1};
    for(uint32_t a=0;a<4096;a++)for(uint32_t b=0;b<4096;b++){
        /* Independent source branch comparison of the two arc lengths. */
        int32_t d=0;
        if(a<b){int32_t forward=(int32_t)(b-a),back=(int32_t)(a+4096-b);d=forward<back?forward:-back;}
        else if(a>b){int32_t back=(int32_t)(a-b),forward=(int32_t)(b+4096-a);d=back<forward?-back:forward;}
        for(unsigned j=0;j<sizeof steps/sizeof steps[0];j++){
            int32_t s=steps[j],m=d<0?-d:d;
            uint32_t expected=m<=s?b:(d<0?a-(uint32_t)s:a+(uint32_t)s);
            assert(taxi_traffic_heading_approach(a,b,s)==expected);
        }
    }
    assert(taxi_traffic_heading_approach(8191,4096,16)==4096);
    assert(taxi_traffic_heading_approach(8190,4097,1)==8191);
    puts("Traffic heading: all 16777216 angle pairs x 7 step cases PASS");
}
