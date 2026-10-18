/* Input-only route exploration over native recovered routines. This driver is
   diagnostic policy, not original game AI or physics. No actor relocation. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "challenge_ball_course.h"
#include "runtime_asset.h"
#include "game_start.h"
typedef struct {TaxiDriveWorld car;TaxiDriveWorldContext context;TaxiPassengerState actors[20];TaxiChallengeBall timer;TaxiBallStatus status;} Sim;
static TaxiChallengeBallCourse course;
static TaxiRuntimeAsset runtime;
static int32_t word(const unsigned char *p){uint32_t u=p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);int32_t s;memcpy(&s,&u,4);return s;}
static void fail(const char *s){fprintf(stderr,"FAIL %s\n",s);exit(1);}
static void begin(Sim *s){
 memset(s,0,sizeof *s);
 if(taxi_challenge_ball_course_start(&course,&s->car,&s->context,runtime.templates[0],584,128)!=1||taxi_challenge_ball_course_actors(&course,s->actors)!=1)fail("source course init");
 taxi_challenge_ball_begin(&s->timer);
}
static void tick(Sim *s,unsigned mask,int analog,int intro){
 TaxiBallEffects ball;TaxiBallInput gates={.intro=(uint8_t)intro,.delta=1};
 if(taxi_challenge_ball_course_control(&course,&s->timer,s->actors,&s->car,gates,&s->status,&ball,&runtime.trig))fail("native Ball controller");
 s->context.height.flag_1697=s->status.ended;s->context.height.flag_153c=s->status.success;
 TaxiBallProjection p[20]={0};uint32_t visible;
 if(taxi_challenge_ball_course_render(&course,s->actors,s->car.world.height.position,s->car.world.height.position,0,0,160,106,0,1,0,&runtime.trig,p,&visible))fail("source tick animation");
 if(s->status.ended)return;
 TaxiCommand command={0};command.bytes[4]=(uint8_t)(mask&15);uint16_t raw=(uint16_t)analog;command.bytes[12]=(uint8_t)raw;command.bytes[13]=(uint8_t)(raw>>8);
 TaxiGearInput input={.delta=1,.frame_delta=1,.settings=128,.action2=!!(mask&4),.action3=!!(mask&8),.action4=!!(mask&16),.action5=!!(mask&32)};
 TaxiDriveWorldEffects effects;
 int rc=intro?taxi_drive_world_start_step(&s->car,&s->context,&input,0,&runtime.trig,&runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&course.resources.world,&effects):taxi_drive_world_step(&s->car,&s->context,&command,&input,0,&runtime.trig,&runtime.traction,&runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&course.resources.world,&effects);
 if(rc)fail("native car step");
}
static void settle(Sim *s){for(unsigned i=0;i<203;i++)tick(s,4,0,1);}
static unsigned nearest(Sim *s,unsigned last,double penalty,double angle_weight){
 unsigned best=20;double score=1e99;
 for(unsigned i=0;i<20;i++)if((s->actors[i].bytes[0]&1)&&!s->actors[i].bytes[10]){
  double x=(double)word(s->actors[i].bytes+16)-s->car.world.height.position.x,z=(double)word(s->actors[i].bytes+24)-s->car.world.height.position.z;
  double angle=fabs(remainder(-atan2(x,z)*4096/(2*3.141592653589793)-(double)s->car.world.heading_20/4096,4096));
  double value=(x*x+z*z)*(1+angle_weight*angle/512);if(i!=last)value*=penalty;
  if(value<score){score=value;best=i;}
 }
 return best;
}
static double edge(const Sim *s,int a,int b){
 double ax=a<0?s->car.world.height.position.x:word(s->actors[a].bytes+16),az=a<0?s->car.world.height.position.z:word(s->actors[a].bytes+24);
 double bx=word(s->actors[b].bytes+16),bz=word(s->actors[b].bytes+24);return hypot(bx-ax,bz-az);
}
static void ordered_route(const Sim *s,unsigned order[20],uint32_t *random){
 unsigned used=0;int prior=-1;
 for(unsigned at=0;at<20;at++){
  unsigned choices[3]={20,20,20};double distances[3]={1e99,1e99,1e99};
  for(unsigned i=0;i<20;i++)if(!(used&(1u<<i))){double d=edge(s,prior,(int)i);
   for(unsigned k=0;k<3;k++)if(d<distances[k]){for(unsigned j=2;j>k;j--){choices[j]=choices[j-1];distances[j]=distances[j-1];}choices[k]=i;distances[k]=d;break;}
  }
  *random=*random*1664525u+1013904223u;unsigned choice=*random%3;if(choices[choice]==20)choice=0;
  order[at]=choices[choice];used|=1u<<order[at];prior=(int)order[at];
 }
 for(unsigned round=0;round<10;round++){
  int changed=0;
  for(unsigned lo=0;lo<19;lo++)for(unsigned hi=lo+1;hi<20;hi++){
   int before=lo?(int)order[lo-1]:-1;
   double old=edge(s,before,(int)order[lo]),fresh=edge(s,before,(int)order[hi]);
   if(hi<19){old+=edge(s,(int)order[hi],(int)order[hi+1]);fresh+=edge(s,(int)order[lo],(int)order[hi+1]);}
   if(fresh+1<old){for(unsigned l=lo,h=hi;l<h;l++,h--){unsigned tmp=order[l];order[l]=order[h];order[h]=tmp;}changed=1;}
  }
  if(!changed)break;
 }
}
int main(int argc,char **argv){
 if(taxi_challenge_ball_course_load(&course,"outputs/crazy-taxi-native/assets")||taxi_runtime_asset_load(&runtime,"outputs/crazy-taxi-native/assets/driving.ctr"))fail("original assets");
 Sim initial;begin(&initial);settle(&initial);
 Sim baseline=initial;for(unsigned i=0;i<397;i++)tick(&baseline,4|(i==0?16:0),0,0);
 printf("BASELINE native600 position%d,%d,%d speed%d popped%d remaining%d\n",baseline.car.world.height.position.x,baseline.car.world.height.position.y,baseline.car.world.height.position.z,baseline.car.world.speed_544,baseline.timer.popped_1556,baseline.timer.remaining_1558);
 if(baseline.car.world.height.position.x!=-1||baseline.car.world.height.position.y!=73||baseline.car.world.height.position.z!=19742||baseline.timer.popped_1556!=1||baseline.timer.remaining_1558!=2303)fail("headless baseline differs from final native viewer600");
 Sim best_sim=initial;unsigned best_count=0,best_ticks=2701;unsigned char best_masks[2700];int16_t best_analog[2700];
 uint32_t random=1234567;unsigned candidates=argc>1?(unsigned)atoi(argv[1]):1024;if(!candidates||candidates>20000)fail("candidate count1..20000");
 for(unsigned candidate=0;candidate<candidates;candidate++){
  random=random*1664525u+1013904223u;int speed=16000+(int)(random%32001);
  random=random*1664525u+1013904223u;double gain=0.04+(random%600)*0.001;
  random=random*1664525u+1013904223u;double penalty=1+(random%100)*0.01;
  random=random*1664525u+1013904223u;double angle_weight=(random%401)*0.01;
  random=random*1664525u+1013904223u;double damping=(random%201)*0.1;
  random=random*1664525u+1013904223u;double corner_slow=(random%201)*0.01;
  random=random*1664525u+1013904223u;double lead=(random%101)*0.01;

  unsigned order[20];ordered_route(&initial,order,&random);Sim sim=initial;unsigned char masks[2700];int16_t analogs[2700];unsigned last=20,elapsed=0;double prior_heading=(double)initial.car.world.heading_20/4096;
  while(elapsed<2700&&!sim.status.ended){
   unsigned target=nearest(&sim,last,penalty,angle_weight);
   if(candidate&1){target=20;for(unsigned a=0;a<20;a++)if((sim.actors[order[a]].bytes[0]&1)&&!sim.actors[order[a]].bytes[10]){target=order[a];break;}}
   if(target==20)break;last=target;
   double dx=(double)word(sim.actors[target].bytes+16)-sim.car.world.height.position.x,dz=(double)word(sim.actors[target].bytes+24)-sim.car.world.height.position.z;
   double frames=fmin(80,sqrt(dx*dx+dz*dz)/fmax(20,fabs((double)sim.car.world.speed_544)/256.0));
   dx+=sim.timer.velocity_155c[target][0]/60.0*frames*lead;dz+=sim.timer.velocity_155c[target][2]/60.0*frames*lead;
   double desired=-atan2(dx,dz)*4096/(2*3.141592653589793),heading=(double)sim.car.world.heading_20/4096;
   double error=remainder(desired-heading,4096);double rotation=remainder(heading-prior_heading,4096);prior_heading=heading;int analog=(int)((error-rotation*damping)*gain);if(analog>192)analog=192;if(analog<-192)analog=-192;
   double limit=speed/(1+fabs(error)*corner_slow/512);unsigned mask=(sim.car.world.speed_544>limit?8:4)|(elapsed==0?16:0);
   masks[elapsed]=(unsigned char)mask;analogs[elapsed]=(int16_t)analog;tick(&sim,mask,analog,0);elapsed++;
  }
  unsigned count=(unsigned)sim.timer.popped_1556;
  if(count>best_count||(count==best_count&&elapsed<best_ticks)){
   best_sim=sim;best_count=count;best_ticks=elapsed;memcpy(best_masks,masks,elapsed);memcpy(best_analog,analogs,elapsed*sizeof *analogs);
   printf("BEST candidate%u popped%u elapsed%u speed%d gain%.3f penalty%.2f angle%.2f damping%.1f slow%.2f lead%.2f final%d,%d\n",candidate,count,elapsed,speed,gain,penalty,angle_weight,damping,corner_slow,lead,sim.car.world.height.position.x,sim.car.world.height.position.z);fflush(stdout);
  }
 }
 FILE *f=fopen("work/ball-input-route-ordered.txt","wb");if(!f)fail("output");for(unsigned i=0;i<best_ticks;i++)fprintf(f,"%u %d\n",best_masks[i],best_analog[i]);if(fclose(f))fail("close");
 printf("ROUTE CANDIDATE popped%u/%u elapsed%u; policy inputs only, requires actual viewer replay before qualification.\n",best_count,20u,best_ticks);
 for(unsigned i=0;i<20;i++)if(!best_sim.actors[i].bytes[10])printf("REMAIN actor%u X%d Z%d velocity%d,%d\n",i,word(best_sim.actors[i].bytes+16),word(best_sim.actors[i].bytes+24),best_sim.timer.velocity_155c[i][0],best_sim.timer.velocity_155c[i][2]);
 taxi_challenge_ball_course_free(&course);return 0;
}
