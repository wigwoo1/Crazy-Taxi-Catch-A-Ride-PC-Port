#include "ai_command.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <limits.h>
#include <stdio.h>
int main(void){
    assert(taxi_ai_vector_angle(1,1)==512&&taxi_ai_vector_angle(-1,1)==1536);
    assert(taxi_ai_vector_angle(1,-1)==-512&&taxi_ai_vector_angle(-1,-1)==2560);
    assert(taxi_ai_vector_angle(0,0)==512);
    for(int x=1;x<=127;x++)for(int z=1;z<=127;z++){
        int angle=taxi_ai_vector_angle(x,z);
        int expected=x>z?(z*512/x):1024-x*512/z;
        assert(angle==expected);
        assert(taxi_ai_vector_angle(-x,z)==2048-angle);
        assert(taxi_ai_vector_angle(x,-z)==-angle);
        assert(taxi_ai_vector_angle(-x,-z)==2048+angle);
    }
    for(unsigned a=0;a<4096;a++)for(unsigned b=0;b<4096;b++){
        int32_t d=taxi_ai_angle_difference(a,b);
        assert(d>=-2048&&d<=2048&&(((uint32_t)((int32_t)a+d)&4095)==b));
        assert(d==-taxi_ai_angle_difference(b,a));
        assert(d==taxi_ai_angle_difference(a+4096,b+8192));
    }
    assert(taxi_ai_angle_difference(0,2048)==-2048&&taxi_ai_angle_difference(2048,0)==2048);
    for(unsigned separation=0;separation<4096;separation++)for(unsigned parity=0;parity<2;parity++){
        uint32_t counter=parity;uint16_t query=123;
        int result=taxi_ai_reposition_query(0,separation<<12,&counter,&query);
        unsigned distance=separation>2048?4096-separation:separation;
        assert(result==(distance>384));
        assert(counter==parity+(distance>384));
        assert(query==(distance>384?(parity?4096:61440):123));
    }
    uint32_t counter=UINT32_MAX;uint16_t query=0;
    assert(taxi_ai_reposition_query(0,2048u<<12,&counter,&query)==1&&counter==0&&query==4096);
    const int32_t speeds[]={INT32_MIN,-1,0,99998,99999,100000,INT32_MAX};
    const int32_t errors[]={INT32_MIN,-51,-50,-49,0,49,50,51,INT32_MAX};
    for(unsigned old=0;old<256;old++)for(unsigned s=0;s<7;s++)for(unsigned e=0;e<9;e++){
        uint8_t actual=taxi_ai_command_prefix((uint8_t)old,speeds[s],errors[e]);
        assert((actual&240)==(old&240));
        assert((actual&4)==(speeds[s]<100000?4:0));
        assert((actual&3)==(errors[e]<=-51?2:errors[e]>=51?1:0));
        assert(!(actual&8));
    }
    puts("PASS source AI command prefix: preserved high flags, signed speed threshold, +/-50 steering deadband.");
}
