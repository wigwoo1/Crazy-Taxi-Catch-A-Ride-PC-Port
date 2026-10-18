#include "ai_command.h"
#include <string.h>
static int32_t ai_signed(uint32_t bits){int32_t value;memcpy(&value,&bits,4);return value;}
int32_t taxi_ai_vector_angle(int32_t x,int32_t z){
    unsigned quadrant=(x<0?1u:0u)|(z<0?2u:0u);
    x=x<0?ai_signed(0u-(uint32_t)x):x?x:1;
    z=z<0?ai_signed(0u-(uint32_t)z):z?z:1;
    int greater=x>z;
    int32_t numerator=ai_signed((uint32_t)(greater?z:x)<<16);
    int32_t divisor=greater?x:z;
    int32_t quotient=ai_signed((uint32_t)((int64_t)numerator/divisor));
    int32_t ratio=quotient>=0?quotient/128:(int32_t)(-((-(int64_t)quotient+127)/128));
    int32_t angle=greater?ratio:1024-ratio;
    if(quadrant==1)angle=2048-angle;
    else if(quadrant==2)angle=-angle;
    else if(quadrant==3)angle+=2048;
    return angle;
}
int taxi_ai_reposition_query(uint32_t player,uint32_t vehicle,uint32_t *counter,uint16_t *query){
    if(!counter||!query)return -1;
    int32_t difference=taxi_ai_angle_difference(player>>12,vehicle>>12);
    if(difference>=-384&&difference<=384)return 0;
    uint32_t parity=(*counter)++&1;
    *query=(uint16_t)((player>>8)+(parity?4096u:(uint32_t)-4096));
    return 1;
}
int32_t taxi_ai_angle_difference(uint32_t from,uint32_t to){
    from&=4095;to&=4095;
    int32_t difference=(int32_t)to-(int32_t)from;
    if(difference>=2048)difference-=4096;
    else if(difference<=-2048)difference+=4096;
    return difference;
}
uint8_t taxi_ai_command_prefix(uint8_t previous,int32_t speed,int32_t error){
    uint8_t command=(uint8_t)(previous&240);
    if(speed<=99999)command|=4;
    if(error < -50)command|=2;
    else if(error > 50)command|=1;
    return command;
}
