#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "challenge_ball.h"
static void put(uint8_t *p,int32_t v){uint32_t u=(uint32_t)v;for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(u>>(8*i));}
static int32_t get(const uint8_t *p){uint32_t u=(uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;int32_t s;memcpy(&s,&u,4);return s;}
static uint32_t root(uint32_t n){uint32_t r=0,b=1u<<30;while(b>n)b>>=2;while(b){if(n>=r+b){n-=r+b;r=(r>>1)+b;}else r>>=1;b>>=2;}return r;}
int main(void){
 assert(taxi_challenge_ball_impulse_angle(0,0)==512);
 assert(taxi_challenge_ball_impulse_angle(1000,1000)==512);
 assert(taxi_challenge_ball_impulse_angle(-1000,1000)==1536);
 assert(taxi_challenge_ball_impulse_angle(1000,-1000)==-512);
 assert(taxi_challenge_ball_impulse_angle(-1000,-1000)==2560);
 assert(taxi_challenge_ball_impulse_angle(1000,500)==256);
 assert(taxi_challenge_ball_impulse_angle(500,1000)==768);

 TaxiChallengeBall s;memset(&s,0xaa,sizeof s);taxi_challenge_ball_begin(&s);assert(s.popped_1556==0&&s.remaining_1558==2700);
 for(unsigned i=0;i<20;i++)for(unsigned j=0;j<3;j++)assert(!s.velocity_155c[i][j]);
 for(unsigned i=0;i<20;i++)for(int paused=0;paused<2;paused++){
  uint8_t actor[44],expected[44];memset(actor,0xab,sizeof actor);put(actor+16,100);put(actor+24,-200);memcpy(expected,actor,44);
  s.velocity_155c[i][0]=125;s.velocity_155c[i][1]=9876;s.velocity_155c[i][2]=-125;
  assert(!taxi_challenge_ball_motion_step(&s,actor,i,paused,1));
  if(!paused){put(expected+16,102);put(expected+24,-202);}assert(!memcmp(actor,expected,44));
  assert(s.velocity_155c[i][0]==93&&s.velocity_155c[i][1]==9876&&s.velocity_155c[i][2]==-93);
  assert(!taxi_challenge_ball_motion_step(&s,actor,i,1,3));assert(!s.velocity_155c[i][0]&&!s.velocity_155c[i][2]);
 }
 uint8_t a[44]={0};s.velocity_155c[0][0]=59;s.velocity_155c[0][2]=-59;
 assert(!taxi_challenge_ball_motion_step(&s,a,0,0,0));assert(get(a+16)==0&&get(a+24)==0);assert(s.velocity_155c[0][0]==59&&s.velocity_155c[0][2]==-59);
 put(a+16,INT32_MAX);put(a+24,INT32_MIN);s.velocity_155c[0][0]=120;s.velocity_155c[0][2]=-120;
 assert(!taxi_challenge_ball_motion_step(&s,a,0,0,1));assert(get(a+16)==INT32_MIN+1&&get(a+24)==INT32_MAX-1);
 TaxiChallengeBall before=s;uint8_t old[44];memcpy(old,a,44);assert(taxi_challenge_ball_motion_step(&s,a,20,0,1)==-1);assert(!memcmp(&s,&before,sizeof s)&&!memcmp(a,old,44));
 assert(taxi_challenge_ball_motion_step(NULL,a,0,0,1)==-1);assert(taxi_challenge_ball_motion_step(&s,NULL,0,0,1)==-1);
 uint8_t actors[20][44]={{0}};int32_t xt[4096],zt[4096];for(unsigned i=0;i<4096;i++){xt[i]=256;zt[i]=-256;}
 taxi_challenge_ball_begin(&s);actors[0][0]=actors[1][0]=actors[2][0]=actors[3][0]=1;
 put(actors[1]+16,1000);put(actors[2]+16,25600);actors[3][10]=1;
 s.velocity_155c[1][1]=42;assert(!taxi_challenge_ball_impulse(&s,actors,0,xt,zt,root));
 assert(s.velocity_155c[1][0]==2460&&s.velocity_155c[1][2]==-2460&&s.velocity_155c[1][1]==42);
 assert(!s.velocity_155c[0][0]&&!s.velocity_155c[2][0]&&!s.velocity_155c[3][0]);
 before=s;assert(taxi_challenge_ball_impulse(&s,actors,20,xt,zt,root)==-1);assert(!memcmp(&s,&before,sizeof s));
 taxi_challenge_ball_begin(&s);memset(actors,0,sizeof actors);for(unsigned i=0;i<20;i++)actors[i][0]=1;
 TaxiBallInput in={.delta=1,.effects_enabled=1};TaxiBallStatus status={0};TaxiBallEffects effects;
 assert(!taxi_challenge_ball_step(&s,actors,&in,&status,&effects,xt,zt,root));
 assert(s.remaining_1558==2699&&s.popped_1556==20&&status.success&&status.ended&&effects.request_rank&&effects.popped_mask==0xfffff&&effects.pop_sound_count==20);
 for(unsigned i=0;i<20;i++)assert(actors[i][10]==1&&actors[i][12]==15&&!actors[i][13]&&actors[i][9]==15);
 assert(!taxi_challenge_ball_step(&s,actors,&in,&status,&effects,xt,zt,root));assert(!effects.popped_mask&&!effects.request_rank&&s.remaining_1558==2699);
 taxi_challenge_ball_begin(&s);memset(actors,0,sizeof actors);status=(TaxiBallStatus){0};s.remaining_1558=600;
 assert(!taxi_challenge_ball_step(&s,actors,&in,&status,&effects,xt,zt,root));assert(s.remaining_1558==599&&effects.warning_palette&&effects.warning_sound);
 s.remaining_1558=1;actors[0][0]=1;assert(!taxi_challenge_ball_step(&s,actors,&in,&status,&effects,xt,zt,root));assert(status.ended&&!status.success&&!s.remaining_1558&&!actors[0][10]);
 taxi_challenge_ball_begin(&s);status=(TaxiBallStatus){0};in.intro=1;
 assert(!taxi_challenge_ball_step(&s,actors,&in,&status,&effects,xt,zt,root));assert(s.remaining_1558==2700&&actors[0][10]&&s.popped_1556==1);
 in.intro=0;in.pause=1;before=s;assert(!taxi_challenge_ball_step(&s,actors,&in,&status,&effects,xt,zt,root));assert(!memcmp(&s,&before,sizeof s)&&!effects.popped_mask);
 in.pause=0;in.effects_enabled=0;s.remaining_1558=600;assert(!taxi_challenge_ball_step(&s,actors,&in,&status,&effects,xt,zt,root));assert(effects.warning_palette&&!effects.warning_sound);
 in.phase=1;before=s;assert(!taxi_challenge_ball_step(&s,actors,&in,&status,&effects,xt,zt,root));assert(effects.render_world&&!memcmp(&s,&before,sizeof s));
 uint8_t sprite[44]={0};sprite[0]=1;sprite[1]=3;TaxiBallRenderRequest request;
 assert(taxi_challenge_ball_render_request(sprite,0,0,735,0,1,0,&request)==1);assert(request.bitmap_identity==0x0849bcc9&&request.y==735&&request.scale==2178048&&sprite[32]==32&&sprite[33]==32);
 sprite[10]=1;sprite[8]=0;sprite[9]=15;sprite[12]=15;sprite[13]=0;
 for(unsigned frame=0;frame<4;frame++)for(unsigned tick=0;tick<15;tick++){
  assert(taxi_challenge_ball_render_request(sprite,0,0,735,0,1,0,&request)==1);assert(request.bitmap_identity==0x0849b0c9u+1024u*(frame+4));
 }
 assert(sprite[8]==4&&(sprite[0]&3)==2);TaxiBallRenderRequest retained=request;assert(taxi_challenge_ball_render_request(sprite,0,0,735,0,1,0,&request)==0&&!memcmp(&request,&retained,sizeof request));
 sprite[0]=1;sprite[8]=0;sprite[12]=15;assert(taxi_challenge_ball_render_request(sprite,0,0,735,1,100,0,&request)==1);assert(sprite[8]==0&&sprite[12]==15);
 assert(taxi_challenge_ball_render_request(sprite,25600,0,735,0,1,0,&request)==0);assert(sprite[12]==15);
 puts("PASS sourceBall draw request:32square/type bitmap/default source scale,60 renderer ticks/four pop frames/hide-after-submit, paused/offrange animation gating and no output mutation on cull. GPU/live drawing pending.");
 puts("PASS source8234 controlled timer/contact traversal:20 contacts/single success rank request, no repeated pop, warning crossing, timeout-before-contact, intro contact gate, phase1 render request. No live gameplay/original execution oracle.");
 puts("PASS source861C controlled impulse: self/active/popped/radius gates, overwritten X/Z only, caller Y preserved, source linear angle quadrants; mathematical root fixture, no BIOS execution oracle.");
 puts("PASS Crazy Ball original45-second/20-vector initialization and86FC motion:40 actor/pause cases, signed division, wrap32, friction while paused, caller bytes/Y velocity preserved, invalid atomicity. Live course/render not integrated.");return 0;
}
