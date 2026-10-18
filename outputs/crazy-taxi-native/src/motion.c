#include "motion.h"
#include <string.h>
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
static int32_t add(int32_t a,int32_t b) {return bits((uint32_t)a+(uint32_t)b);}
static int32_t sub(int32_t a,int32_t b) {return bits((uint32_t)a-(uint32_t)b);}
static int32_t mul(int32_t a,int32_t b) {return bits((uint32_t)a*(uint32_t)b);}
static int32_t asr(int32_t a,unsigned n) {return bits(((uint32_t)a>>n)|(a<0?(UINT32_MAX<<(32-n)):0));}
static int16_t narrow(int32_t a) {uint16_t u=(uint16_t)a;int16_t s;memcpy(&s,&u,2);return s;}
static int32_t divide(int32_t a,int32_t b) {return bits((uint32_t)((int64_t)a/b));}
static int prepare(TaxiMotionState *v,TaxiOrientationContext *c,const TaxiCommand *command,
    const TaxiTrig *t,TaxiOrientationEffects *e) {
    if(!c->delta) return -1;
    TaxiOrientation *o=&v->orientation;
    v->steering.speed_544=o->speed_544;v->steering.state_564=o->state_564;
    v->steering.phase_e6a=o->phase_e6a;v->steering.flags_28=o->flags_28;
    v->steering.flags_358=o->flags_358;
    int32_t response;
    if(taxi_steering_response(&v->steering,command,c->settings,t,&response)<0) return -1;
    o->flags_28=v->steering.flags_28;v->response_57c=narrow(response);
    int32_t speed=asr(mul(asr(mul(o->speed_544,v->scale_7c8),12),v->scale_e84),12);
    TaxiPoint old_pivot={0},new_pivot={0};int32_t radius=0,target=0;
    int turning=v->response_57c && o->speed_544;
    if(turning) {
        if(v->response_57c>0 && v->response_57c<11) v->response_57c=11;
        if(v->response_57c<0 && v->response_57c>-11) v->response_57c=-11;
        int32_t sine=mul(t->sine[(uint16_t)v->response_57c&4095],16);
        if(!sine) return -1;
        radius=sub(divide(mul(mul(v->dimension_594,128)/1000,4096),sine),2);
        int32_t circumference=asr(mul(mul(radius,2),12867),7);
        if(!circumference) return -1;
        int32_t angle=divide(mul(mul(c->delta,speed)/60,4096),circumference);
        old_pivot=taxi_rotate_point((TaxiPoint){0,0,216},o->angle_18,asr(o->heading_1c,12),o->angle_1a,t);
        o->heading_1c=add(o->heading_1c,mul(angle,4096));
        new_pivot=taxi_rotate_point((TaxiPoint){0,0,216},o->angle_18,asr(o->heading_1c,12),o->angle_1a,t);
    }
    taxi_orientation_step(o,c,t,e);
    TaxiPoint step=taxi_rotate_point((TaxiPoint){0,0,mul(mul(c->delta,256),speed)/60},
        o->angle_18,asr(o->heading_20,12),o->angle_1a,t);
    v->displacement=(TaxiPoint){add(asr(step.x,12),sub(new_pivot.x,old_pivot.x)),
        sub(0,add(asr(step.y,12),sub(new_pivot.y,old_pivot.y))),
        add(asr(step.z,12),sub(new_pivot.z,old_pivot.z))};
    if(turning) {
        int32_t denominator=mul(radius,40181)/32768;
        if(!denominator) return -1;
        int32_t s=asr(o->speed_544,4);target=divide(mul(s,s),denominator);
    }
    /* Original signed divide8032488 returns0 for denominator0. */
    int32_t damping_period=8/c->delta;
    o->force_540=add(o->force_540,damping_period?divide(sub(target,o->force_540),damping_period):0);
    return 0;
}
static TaxiMotionObserver observer;
void taxi_motion_observer(TaxiMotionObserver value){observer=value;}
int taxi_motion_prepare(TaxiMotionState *state,TaxiOrientationContext *context,
    const TaxiCommand *command,const TaxiTrig *trig,TaxiOrientationEffects *effects) {
    if(!state || !context || !command || !trig || !effects) return -1;
    TaxiMotionState v=*state;TaxiOrientationContext c=*context;TaxiOrientationEffects e={0};
    if(prepare(&v,&c,command,trig,&e)<0) return -1;
    if(observer)observer(state,context,command,&v);
    *state=v;*context=c;*effects=e;return 0;
}

