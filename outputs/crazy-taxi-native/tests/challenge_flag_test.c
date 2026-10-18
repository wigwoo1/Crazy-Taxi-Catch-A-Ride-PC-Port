#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include "challenge_flag.h"
int main(void){
 TaxiChallengeFlag s;TaxiChallengeFlagStatus status={0};TaxiChallengeFlagEffects e;TaxiChallengeFlagInput in={.delta=1,.car_x=768,.effects_enabled=1};
 taxi_challenge_flag_begin(&s);assert(s.remaining_1552==1200&&s.scale_1554==256&&!s.caught_1550);
 for(int tick=0;tick<1200;tick++){taxi_challenge_flag_step(&s,&in,&status,&e);assert(e.warning_palette==(tick==600));assert(e.sound_index==(tick==600?39:-1));}
 assert(!s.remaining_1552&&status.ended1697&&!status.success153c);
 taxi_challenge_flag_begin(&s);status=(TaxiChallengeFlagStatus){0};in.car_x=767;in.car_z=-767;taxi_challenge_flag_step(&s,&in,&status,&e);
 assert(s.caught_1550&&status.success153c&&status.ended1697&&e.request_rank&&e.sound_index==42&&s.scale_1554==255);
 for(int tick=0;tick<239;tick++)taxi_challenge_flag_step(&s,&in,&status,&e);assert(s.scale_1554==16&&e.hide_flag);
 for(unsigned gate=0;gate<4;gate++){taxi_challenge_flag_begin(&s);status=(TaxiChallengeFlagStatus){0};in=(TaxiChallengeFlagInput){.delta=1,.car_x=1000};if(gate==0)in.phase153a=1;if(gate==1)in.outer1ea8=1;if(gate==2)in.intro1696=1;if(gate==3)in.pause5f20=1;taxi_challenge_flag_step(&s,&in,&status,&e);assert(s.remaining_1552==1200);}
 taxi_challenge_flag_begin(&s);status=(TaxiChallengeFlagStatus){0};in=(TaxiChallengeFlagInput){.delta=1200};taxi_challenge_flag_step(&s,&in,&status,&e);assert(status.ended1697&&!status.success153c&&!e.request_rank&&e.warning_palette&&e.sound_index==-1);
 taxi_challenge_flag_begin(&s);status=(TaxiChallengeFlagStatus){0};in=(TaxiChallengeFlagInput){.delta=1,.intro1696=1};taxi_challenge_flag_step(&s,&in,&status,&e);assert(status.success153c&&s.remaining_1552==1200&&e.request_rank);
 uint8_t actor[44]={1};actor[16]=0xda;actor[17]=0x56;actor[18]=1;actor[24]=0xfd;actor[25]=actor[26]=actor[27]=255;
 TaxiChallengeFlagRenderRequest request={0};
 assert(taxi_challenge_flag_render_request(actor,87770,-3,735,256,0,&request)==1);
 assert(request.x==87770&&request.y==735&&request.z==-3&&request.scale==1452032&&request.rotation_angle==4096&&request.bitmap_identity==0x0849a6c1);
 assert(actor[36]==0xc1&&actor[37]==0xa6&&actor[38]==0x49&&actor[39]==8);
 assert(taxi_challenge_flag_render_request(actor,87770+25599,-3,735,16,256,&request)==1&&request.scale==32&&request.rotation_angle==256);
 assert(!taxi_challenge_flag_render_request(actor,87770+25600,-3,735,256,0,&request));actor[0]=2;assert(!taxi_challenge_flag_render_request(actor,87770,-3,735,256,0,&request));
 TaxiTrig trig={0};trig.cosine[0]=256;trig.sine[1024]=256;TaxiPoint corners[4];
 request.scale=65536;request.rotation_angle=4096;
 assert(!taxi_challenge_flag_corners(&request,(TaxiPoint){100,200,300},&trig,corners));
 assert(corners[0].x==84&&corners[0].y==200&&corners[1].x==84&&corners[1].y==264&&corners[2].x==116&&corners[3].y==200&&corners[3].z==300);
 request.rotation_angle=1024;assert(!taxi_challenge_flag_corners(&request,(TaxiPoint){100,200,300},&trig,corners));
 assert(corners[0].x==100&&corners[0].y==184&&corners[1].x==36&&corners[1].y==184&&corners[2].x==36&&corners[2].y==216&&corners[3].x==100&&corners[3].y==216);
 assert(taxi_challenge_flag_corners(0,(TaxiPoint){0},&trig,corners)==-1);
 puts("PASS native Crazy Flag scalar callback: original1200 timer,600 boundary,767 square contact, expiry-before-contact, source gates and collapse16.");return 0;
}
