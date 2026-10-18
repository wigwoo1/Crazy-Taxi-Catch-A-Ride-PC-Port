#include "camera_angle.h"
#include "engine_rotate_owned.h"
#include "orientation.h"
#include <string.h>
static int32_t bits(uint32_t u) {int32_t s;memcpy(&s,&u,4);return s;}
static int32_t add(int32_t a,int32_t b) {return bits((uint32_t)a+(uint32_t)b);}
static int32_t sub(int32_t a,int32_t b) {return bits((uint32_t)a-(uint32_t)b);}
static int32_t mul(int32_t a,int32_t b) {return bits((uint32_t)a*(uint32_t)b);}
static int32_t asr12(int32_t a) {return bits(((uint32_t)a>>12)|(a<0?0xfff00000u:0));}
int taxi_camera_project(TaxiPoint *screen,TaxiPoint anchor,TaxiPoint camera,
    int16_t pitch,int16_t yaw,int16_t roll,int32_t focal,const TaxiTrig *trig) {
    TaxiPoint p={sub(anchor.x,camera.x),sub(anchor.y,camera.y),sub(anchor.z,camera.z)};
    if(taxi_engine_rotate_owned(p,-(int32_t)pitch,-(int32_t)yaw,-(int32_t)roll,trig,1,&p))return -1;
    if(!p.z) return 0;
    int32_t x=bits((uint32_t)((int64_t)mul(focal,p.x)/p.z));
    int32_t y=bits((uint32_t)((int64_t)mul(focal,p.y)/p.z));
    *screen=(TaxiPoint){add(120,x),sub(80,y),p.z};return 1;
}
int taxi_camera_command(uint16_t *flags,uint8_t command) {
    uint16_t before=*flags;
    if(command&1) *flags|=16;
    else {
        *flags&=(uint16_t)~16u;
        if(command&2) {
            *flags=(uint16_t)(*flags?*flags+1:2);
            if(*flags==4) *flags=0;
        }
    }
    return !!((before^*flags)&16);
}
int32_t taxi_camera_transform_yaw(int32_t camera,int32_t offset) {
    /* Source15500 stores the signed sum directly as a halfword. */
    return add(asr12(camera),offset);
}
TaxiPoint taxi_camera_position(TaxiPoint taxi,int32_t pitch,int32_t yaw,
    int32_t height,int32_t distance,int32_t base,const TaxiTrig *trig) {
    TaxiPoint offset={0,asr12(height),sub(base,asr12(distance))};
    if(taxi_engine_rotate_owned(offset,asr12(pitch),asr12(yaw),0,trig,0,&offset))return (TaxiPoint){0};
    return (TaxiPoint){add(taxi.x,offset.x),add(taxi.y,offset.y),add(taxi.z,offset.z)};
}
void taxi_camera_mode2_offsets(TaxiCameraOffsets *s,int32_t scale,uint8_t flags,
    int32_t delta,uint8_t freeze,int32_t preset_y,int32_t preset_z) {
    int32_t numerator=bits((uint32_t)sub(scale,4096)<<9);
    int32_t target=flags&32?bits(0xffe80000u):bits((uint32_t)(numerator/614)<<12);
    if(!freeze) {
        int32_t step=bits((uint32_t)delta<<15);
        if(s->zoom_2628<target) {s->zoom_2628=add(s->zoom_2628,step);if(s->zoom_2628>target)s->zoom_2628=target;}
        else if(s->zoom_2628>target) {s->zoom_2628=sub(s->zoom_2628,step);if(s->zoom_2628<target)s->zoom_2628=target;}
    }
    s->height_76c=mul(468,preset_y);
    s->distance_770=add(bits((uint32_t)preset_z<<10),s->zoom_2628);
}
TaxiPoint taxi_camera_rear_position(TaxiPoint taxi,int32_t pitch,int32_t yaw,
    int32_t offset690,const TaxiTrig *trig) {
    TaxiPoint offset={0,640,add(offset690,256)};
    if(taxi_engine_rotate_owned(offset,asr12(pitch),asr12(yaw),0,trig,0,&offset))return (TaxiPoint){0};
    return (TaxiPoint){add(taxi.x,offset.x),add(taxi.y,offset.y),add(taxi.z,offset.z)};
}
static int32_t follow(int32_t camera,int32_t heading,int32_t delta,
    int32_t limit,int32_t rate,const TaxiTrig *trig) {
    int32_t difference=taxi_angle_difference(asr12(heading),asr12(camera));
    if(difference < -limit) camera=sub(camera,mul(add(difference,limit),4096));
    else if(difference > limit) camera=sub(camera,mul(sub(difference,limit),4096));
    /* The original deliberately retains the pre-clamp difference. */
    unsigned index=(unsigned)(difference<0?-difference:difference)&4095;
    int32_t amount=mul(mul(trig->sine[index],16),mul(delta,rate));
    if(difference<0) {
        camera=add(camera,amount);difference=add(difference,asr12(amount));
        if(difference>0) camera=add(camera,mul(difference,4096));
    } else if(difference>0) {
        camera=sub(camera,amount);difference=sub(difference,asr12(amount));
        if(difference<0) camera=sub(camera,mul(difference,4096));
    }
    return camera;
}
int32_t taxi_camera_yaw_step(int32_t camera,int32_t heading,int32_t delta,
    int alternate_limit,const TaxiTrig *trig) {
    return follow(camera,heading,delta,alternate_limit?1991:853,24,trig);
}
int32_t taxi_camera_pitch_step(int32_t camera,int16_t body_pitch,int32_t delta,const TaxiTrig *trig) {
    return follow(camera,mul(-(int32_t)(uint16_t)body_pitch,4096),delta,853,48,trig);
}
int taxi_camera_select_mode(uint16_t *flags,int32_t humans,int demo){
    if(!flags)return -1;
    if(demo)humans=1;
    if(!humans)*flags=1;
    else if(humans!=1)return -1;
    uint16_t mode=(uint16_t)(*flags&~16u);
    return mode<=3?(int)mode:-1;
}
int taxi_camera_fixed_offsets(unsigned mode,TaxiCameraOffsets *out,int32_t *roll){
    if(!out||!roll||mode>1)return -1;
    out->height_76c=mode?0x14c000:0x159000;
    out->distance_770=mode?0x280000:0x333000;
    *roll=0x4f000;return 0;
}
int32_t taxi_camera_yaw_offset_step(int32_t current,int32_t target,int32_t delta){
    int32_t step=bits((uint32_t)delta<<5);
    if(current>target){current=sub(current,step);if(current<=target)current=target;}
    else if(current<target){current=add(current,step);if(current>=target)current=target;}
    return current;
}

void taxi_camera_mode3_offsets(TaxiCameraOffsets *state,int32_t base,
    int32_t offset,int32_t *roll,uint8_t *flags) {
    uint32_t neg=0u-(uint32_t)offset;
    int32_t half=bits((neg>>1)|((neg&0x80000000u)?0x80000000u:0));
    *roll=0x4f000;
    state->height_76c=0x159000;
    state->distance_770=bits((uint32_t)add(base,half)<<12);
    *flags|=2;
}

TaxiCameraView taxi_camera_mode3_view(TaxiPoint taxi,int16_t pitch,
    int32_t heading,int16_t roll,TaxiPoint corrections,int32_t offset,const TaxiTrig *trig) {
    TaxiCameraView view;
    uint16_t a=(uint16_t)add(pitch,corrections.x),b=(uint16_t)asr12(add(heading,corrections.y)),c=(uint16_t)add(roll,corrections.z);
    memcpy(&view.pitch,&a,2);memcpy(&view.yaw,&b,2);memcpy(&view.roll,&c,2);
    int32_t half=bits(((uint32_t)offset>>1)|(offset<0?0x80000000u:0));
    TaxiPoint local=taxi_rotate_point((TaxiPoint){0,345,half},view.pitch,view.yaw,view.roll,trig);
    view.position=(TaxiPoint){add(taxi.x,local.x),add(taxi.y,local.y),add(taxi.z,local.z)};
    return view;
}

TaxiCameraView taxi_camera_normal_view(TaxiPoint taxi,int32_t pitch,int32_t yaw,
    int32_t offset,const TaxiCameraOffsets *offsets,int32_t base,const TaxiTrig *trig) {
    TaxiCameraView view={.position=taxi_camera_position(taxi,pitch,yaw,
        offsets->height_76c,offsets->distance_770,base,trig),.roll=0};
    uint16_t p=(uint16_t)asr12(pitch),y=(uint16_t)taxi_camera_transform_yaw(yaw,offset);
    memcpy(&view.pitch,&p,2);memcpy(&view.yaw,&y,2);return view;
}

TaxiCameraView taxi_camera_rear_view(TaxiPoint taxi,int32_t pitch,int32_t yaw,int32_t offset,const TaxiTrig *trig){
 TaxiCameraView view={.position=taxi_camera_rear_position(taxi,pitch,yaw,offset,trig),.pitch=136,.roll=0};
 uint16_t y=(uint16_t)add(asr12(yaw),2048);memcpy(&view.yaw,&y,2);return view;
}

TaxiCameraView taxi_camera_mode2_view(TaxiPoint taxi,int32_t pitch,int32_t yaw,int32_t offset,const TaxiCameraOffsets *offsets,int32_t base,int32_t rear_offset,uint16_t flags,uint8_t *actor_flags,const TaxiTrig *trig){
 if(flags&16){*actor_flags&=253;return taxi_camera_rear_view(taxi,pitch,yaw,rear_offset,trig);}
 return taxi_camera_normal_view(taxi,pitch,yaw,offset,offsets,base,trig);
}
