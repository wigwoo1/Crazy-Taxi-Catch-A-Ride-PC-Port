#include "steering_input.h"
#include <string.h>
static int16_t narrow(uint32_t u) {uint16_t h=(uint16_t)u;int16_t s;memcpy(&s,&h,2);return s;}
void taxi_steering_input(TaxiGearState *v,TaxiCommand *command,int16_t rate,
    int32_t delta,uint8_t freeze,int32_t phase_timer) {
    int32_t analog=narrow(command->bytes[12]|((uint32_t)command->bytes[13]<<8));
    if(analog>=-7&&analog<=7) analog=0;
    if(analog) {
        command->bytes[4]=(uint8_t)((command->bytes[4]&~3u)|(analog<0?1:2));
        v->steering_578=narrow((uint32_t)analog*8);
        if(v->steering_578>1536) v->steering_578=1536;
        if(v->steering_578<-1536) v->steering_578=-1536;
        return;
    }
    if(freeze) return;
    uint32_t step=(uint32_t)(int32_t)rate*(uint32_t)delta;
    if(command->bytes[4]&1) {
        v->steering_578=narrow((uint32_t)(int32_t)v->steering_578+step);
        if(v->steering_578>1536) v->steering_578=1536;
    } else if(command->bytes[4]&2) {
        v->steering_578=narrow((uint32_t)(int32_t)v->steering_578-step);
        if(v->steering_578<-1536) v->steering_578=-1536;
    } else {
        int16_t old=v->steering_578;
        if(old>0) {v->steering_578=narrow((uint32_t)old-step);if(v->steering_578<0) v->steering_578=0;}
        else if(old<0) {v->steering_578=narrow((uint32_t)(int32_t)old+step);if(v->steering_578>0) v->steering_578=0;}
        return;
    }
    if(phase_timer&&v->phase_e6a==1) {v->phase_e6a=2;v->phase_e6b=0;}
}
