#include "passenger_idle.h"
#include "random.h"
#include "passenger_frame_banks.h"
#include <string.h>
static FILE *idle_trace;
void taxi_passenger_idle_trace(FILE *sink){idle_trace=sink;}
static int32_t bits(uint32_t u){int32_t s;memcpy(&s,&u,4);return s;}
static int32_t word(const uint8_t *p){return bits(p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24));}
static int32_t half(const uint8_t *p){unsigned u=p[0]|((unsigned)p[1]<<8);return u<32768?(int32_t)u:(int32_t)u-65536;}
static int32_t byte(uint8_t u){return u<128?u:(int32_t)u-256;}
static void put_half(uint8_t *p,int32_t v){p[0]=(uint8_t)v;p[1]=(uint8_t)((uint32_t)v>>8);}
static void put_word(uint8_t *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(8*i));}
static int32_t absdiff(int32_t a,int32_t b){int32_t v=bits((uint32_t)a-(uint32_t)b);return v<0?bits(0u-(uint32_t)v):v;}
static int idle(TaxiPassengerState *out,int selected,const TaxiPickupState *pickup,
    TaxiRoutePoint taxi,int32_t delta,uint8_t freeze,int32_t random,int32_t delay,uint32_t *rng){
    if(!out||!pickup)return -1;
    TaxiPassengerState state=*out;uint8_t *p=state.bytes;
    if(absdiff(taxi.x,word(p+16))>0x31ff||absdiff(taxi.z,word(p+24))>0x31ff||byte(p[1])>7)return 0;
    if(p[0]&64){int32_t next=bits((uint32_t)p[11]+(uint32_t)delta);p[11]=(uint8_t)next;if(byte(p[11])>89)p[11]=90;}
    if((p[0]&3)==1&&!freeze){
        int32_t stage=byte(p[10]);
        if(stage>=0&&stage<=2&&!(selected&&pickup->pending_1704)){
            put_half(p+12,bits((uint32_t)half(p+12)-(uint32_t)delta));
            if(half(p+12)<=0){
                if(stage==0){
                    if(rng)random=taxi_random_range_tagged(rng,0,65535,"passenger-idle-choice");
                    p[10]=(uint8_t)((random&1)+1);put_half(p+12,10);p[9]=10;
                    put_word(p+28,p[10]==1?0x087ec484:0x087ec4bc);put_half(p+14,0);p[8]=0;
                }else{
                    if(word(p+28)!=(stage==1?0x087ec484:0x087ec4bc))return -1;
                    put_half(p+12,byte(p[9]));int32_t index=half(p+14)+1;put_half(p+14,index);
                    if(index<0||index>13)return -1;
                    static const uint8_t frames[13]={0,1,2,3,2,1,0,1,2,3,2,1,0};
                    if(index<13)p[8]=frames[index];
                    else{
                        if(rng)delay=taxi_random_range_tagged(rng,120,600,"passenger-idle-delay");
                        p[10]=0;put_half(p+12,delay);p[8]=0;put_half(p+14,0);put_word(p+28,0x087ec47c);
                        if((p[0]&64)&&byte(p[11])==90)p[0]&=190;
                    }
                }
            }
        }
    }
    *out=state;return 0;
}
int taxi_passenger_idle(TaxiPassengerState *out,int selected,const TaxiPickupState *pickup,
    TaxiRoutePoint taxi,int32_t delta,uint8_t freeze,int32_t random,int32_t delay){
    return idle(out,selected,pickup,taxi,delta,freeze,random,delay,0);
}
int taxi_passenger_idle_rng(TaxiPassengerState *out,int selected,const TaxiPickupState *pickup,
    TaxiRoutePoint taxi,int32_t delta,uint8_t freeze,uint32_t *rng){
    if(!rng)return -1;
    uint32_t before=*rng,next=*rng;
    TaxiPassengerState prior;
    if(idle_trace&&out)prior=*out;
    int status=idle(out,selected,pickup,taxi,delta,freeze,0,0,&next);
    if(!status)*rng=next;
    if(idle_trace&&out&&pickup&&before!=*rng){
        fprintf(idle_trace,"%d %ld %ld %ld %ld %u %u %u %d",selected,
            (long)pickup->pending_1704,(long)taxi.x,(long)taxi.z,
            (long)delta,(unsigned)freeze,before,*rng,status);
        for(unsigned i=0;i<sizeof prior.bytes;i++)fprintf(idle_trace," %u",(unsigned)prior.bytes[i]);
        for(unsigned i=0;i<sizeof out->bytes;i++)fprintf(idle_trace," %u",(unsigned)out->bytes[i]);
        fputc('\n',idle_trace);
    }
    return status;
}

/* Original0802318E..231AA; ROM087EC51C kind/stage table. */
int taxi_passenger_frame_select(TaxiPassengerState *out){
    if(!out||out->bytes[1]>=8||out->bytes[10]>=4)return -1;

    uint32_t address=taxi_passenger_frame_bases[out->bytes[1]*4+out->bytes[10]]+(uint32_t)byte(out->bytes[8])*2048u;
    put_word(out->bytes+36,address);return 0;
}

/* Original22E38..22ECA: all arithmetic stores bytes before signed tests. */
int taxi_passenger_marker_phase_step(TaxiPassengerMarkerPhase *out,int32_t delta,uint8_t freeze){
    if(!out)return -1;
    if(freeze)return 0;
    TaxiPassengerMarkerPhase s=*out;
    s.clock_5f28=(uint8_t)((uint32_t)s.clock_5f28+(uint32_t)delta);
    if(byte(s.clock_5f28)>6){
        s.clock_5f28=0;
        if(byte(s.direction_5f2a)==1){
            s.frame_5f29=(uint8_t)(s.frame_5f29+1);
            if(byte(s.frame_5f29)==4){s.frame_5f29=3;s.direction_5f2a=255;}
        }else{
            s.frame_5f29=(uint8_t)(s.frame_5f29-1);
            if(byte(s.frame_5f29)==-1){s.frame_5f29=0;s.direction_5f2a=1;}
        }
    }
    *out=s;return 0;
}

int taxi_passenger_marker_requests(const TaxiPassengerState *p,int32_t pending,int32_t active,uint8_t phase,TaxiPassengerMarkerRequest out[2],unsigned *count){
    if(!p||!out||!count)return -1;
    *count=0;
    if(pending||active||(p->bytes[0]&64)||!(p->bytes[0]&1))return 0;
    int32_t x=word(p->bytes+16),y=word(p->bytes+20),z=word(p->bytes+24);
    out[0]=(TaxiPassengerMarkerRequest){.descriptor=0x087ec5d8u+12u*p->bytes[2],.scale=0xc0000u,.x=x,.y=bits((uint32_t)y+1536u),.z=z};
    uint32_t descriptor=0x087ec59cu;
    if(!(p->bytes[0]&4))descriptor+=(uint32_t)(byte(phase)+1)*12u;
    out[1]=(TaxiPassengerMarkerRequest){.descriptor=descriptor,.scale=0x80000u,.x=x,.y=bits((uint32_t)y+512u),.z=z};
    *count=2;return 0;
}
