#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "challenge_ball_course.h"
#include "runtime_asset.h"
#include "integer_root.h"
static int32_t word(const uint8_t *p){uint32_t u=(uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;int32_t s;memcpy(&s,&u,4);return s;}
int main(void){
 TaxiChallengeBallCourse c={0};assert(!taxi_challenge_ball_course_load(&c,"outputs/crazy-taxi-native/assets"));
 assert(c.resources.world.nv==510&&c.resources.world.np==217&&c.actors.count==20&&c.resources.submode==2);
 FILE *bitmap=fopen("outputs/crazy-taxi-native/assets/challenge-ball/balloon.indices","rb");assert(bitmap);uint8_t indices[8192];assert(fread(indices,1,8192,bitmap)==8192&&fgetc(bitmap)==EOF);fclose(bitmap);assert(!memcmp(indices,c.balloon_indices,8192));
 TaxiPassengerState actors[20];memset(actors,0xab,sizeof actors);for(unsigned i=0;i<20;i++)memset(actors[i].bytes+20,0,4);
 TaxiPassengerState prior[20];memcpy(prior,actors,sizeof prior);assert(taxi_challenge_ball_course_actors(&c,actors)==1);
 for(unsigned i=0;i<20;i++){
  uint8_t *a=actors[i].bytes;assert(a[0]==1&&a[1]==c.actors.source[i][3]&&word(a+16)==word(c.actors.source[i]+4)&&word(a+24)==word(c.actors.source[i]+12));
  assert(!a[8]&&!a[9]&&!a[10]&&!a[12]&&!a[13]&&!a[14]&&!a[15]);assert(word(a+28)==0x087ec47c);
  assert(a[32]==32&&a[33]==64&&a[34]==5&&a[35]==6);
  assert(!memcmp(a+2,prior[i].bytes+2,6)&&a[11]==prior[i].bytes[11]&&!memcmp(a+36,prior[i].bytes+36,8));
  int32_t height;assert(taxi_world_asset_height(&c.resources.world,(TaxiPoint){word(a+16),0,word(a+24)},&height)==1&&word(a+20)==height);
 }
 memcpy(prior,actors,sizeof prior);uint8_t kind=c.actors.source[19][3];c.actors.source[19][3]=255;assert(taxi_challenge_ball_course_actors(&c,actors)==-1&&!memcmp(actors,prior,sizeof prior));c.actors.source[19][3]=kind;
 void *storage=c.resources.world.storage;assert(taxi_challenge_ball_course_load(&c,"missing-source-assets")==-1&&c.resources.world.storage==storage);
 TaxiRuntimeAsset runtime;assert(!taxi_runtime_asset_load(&runtime,"outputs/crazy-taxi-native/assets/driving.ctr"));
 FILE *ctr=fopen("outputs/crazy-taxi-native/assets/driving.ctr","rb");assert(ctr);
 uint8_t raw[16388];assert(fread(raw,1,sizeof raw,ctr)==sizeof raw);fclose(ctr);
 for(unsigned i=0;i<4096;i++)for(unsigned axis=0;axis<2;axis++){
  unsigned at=4+axis*8192+i*2;int value=raw[at]|(int)raw[at+1]<<8;
  if(value>=32768)value-=65536;
  int expected=value>=0?value/16:-((-value+15)/16);
  assert((axis?runtime.trig.cosine[i]:runtime.trig.sine[i])==expected);
 }
 puts("PASS all8192 decoded signedQ12->Q8 ROMtable entries match original2DB54 ASR4 initialization.");
 for(unsigned bank=0;bank<2;bank++)for(unsigned settings=0;settings<32;settings++){
  TaxiDriveWorld vehicle;TaxiDriveWorldContext context;memset(&vehicle,0,sizeof vehicle);memset(&context,0,sizeof context);
  assert(taxi_challenge_ball_course_start(&c,&vehicle,&context,runtime.templates[bank],584,(uint8_t)(settings|128))==1);
  assert(context.driver.motion.orientation.submode==2&&context.driver.motion.orientation.settings==(settings|128));
 }
 /* Independent inequality oracle over every16-bit square boundary plus
    deterministic full-range samples, no floating-point expected answers. */
 for(uint32_t k=0;k<65536;k++){
  uint32_t square=k*k;assert(taxi_integer_root(square)==k);
  if(k)assert(taxi_integer_root(square-1)==k-1);
  if(k<65535)assert(taxi_integer_root((k+1)*(k+1)-1)==k);
 }
 uint32_t sample=0x12345678;
 for(unsigned i=0;i<100000;i++){
  sample=sample*1664525u+1013904223u;uint32_t root=taxi_integer_root(sample);
  assert((uint64_t)root*root<=sample&&(uint64_t)(root+1)*(root+1)>sample);
 }
 assert(taxi_integer_root(UINT32_MAX)==65535);
 puts("PASS unsigned floor root: all65536 square boundaries/100000 full-range samples; functional math only.");
 TaxiDriveWorld vehicle={0};TaxiChallengeBall state;taxi_challenge_ball_begin(&state);
 TaxiBallStatus status={0};TaxiBallEffects effects={0};TaxiBallInput gates={.delta=1,.effects_enabled=1,.car_x=INT32_MAX,.car_z=INT32_MAX};
 assert(taxi_challenge_ball_course_actors(&c,actors)==1);
 vehicle.world.height.position.x=word(actors[0].bytes+16);vehicle.world.height.position.z=word(actors[0].bytes+24);
 uint8_t expected[20][44];for(unsigned i=0;i<20;i++)memcpy(expected[i],actors[i].bytes,44);
 TaxiChallengeBall expected_state=state;TaxiBallStatus expected_status=status;TaxiBallEffects expected_effects;
 TaxiBallInput expected_input=gates;expected_input.car_x=vehicle.world.height.position.x;expected_input.car_z=vehicle.world.height.position.z;
 assert(!taxi_challenge_ball_step(&expected_state,expected,&expected_input,&expected_status,&expected_effects,runtime.trig.sine,runtime.trig.cosine,taxi_integer_root));
 assert(!taxi_challenge_ball_course_control(&c,&state,actors,&vehicle,gates,&status,&effects,&runtime.trig));
 assert(effects.popped_mask&1&&state.popped_1556>=1&&state.remaining_1558==2699);
 assert(!memcmp(&state,&expected_state,sizeof state)&&!memcmp(&status,&expected_status,sizeof status));
 assert(effects.popped_mask==expected_effects.popped_mask&&effects.pop_sound_count==expected_effects.pop_sound_count);
 unsigned moving=0;for(unsigned i=0;i<20;i++){assert(!memcmp(actors[i].bytes,expected[i],44));moving+=!!(state.velocity_155c[i][0]||state.velocity_155c[i][2]);}
 assert(moving);memcpy(prior,actors,sizeof prior);TaxiChallengeBall before=state;
 assert(taxi_challenge_ball_course_control(&c,&state,actors,&vehicle,gates,&status,&effects,NULL)==-1);
 assert(!memcmp(actors,prior,sizeof prior)&&!memcmp(&state,&before,sizeof state));
 puts("PASS actualcourse20actor contact fixture: nativevehicle overrides gates, decoded X/Z axes drive impulses/motion, scalar traversal equivalence and invalid atomicity. Controlled contact, not earned driving.");

 assert(taxi_challenge_ball_course_actors(&c,actors)==1);
 TaxiPoint player;memcpy(&player,actors[0].bytes+16,12);TaxiPoint camera={player.x,player.y,player.z-4000};
 TaxiBallProjection projections[20]={0};uint32_t visible=0;
 assert(!taxi_challenge_ball_course_render(&c,actors,player,camera,0,0,160,106,0,1,0,&runtime.trig,projections,&visible));
 assert(visible&1);assert(projections[0].bitmap_identity==0x0849b0c9u+actors[0].bytes[1]*1024u);
 actors[0].bytes[10]=1;actors[0].bytes[8]=0;actors[0].bytes[9]=15;actors[0].bytes[12]=15;actors[0].bytes[13]=0;
 for(unsigned tick=0;tick<60;tick++){
  assert(!taxi_challenge_ball_course_render(&c,actors,player,camera,0,0,160,106,0,1,0,&runtime.trig,projections,&visible));
  assert(visible&1);assert(projections[0].bitmap_identity==0x0849b0c9u+(4+tick/15)*1024u);
 }
 assert((actors[0].bytes[0]&3)==2);
 assert(!taxi_challenge_ball_course_render(&c,actors,player,camera,0,0,160,106,0,1,0,&runtime.trig,projections,&visible)&&!(visible&1));
 memcpy(prior,actors,sizeof prior);uint32_t old_visible=visible;
 assert(taxi_challenge_ball_course_render(&c,actors,player,camera,0,0,160,106,0,1,0,NULL,projections,&visible)==-1);
 assert(!memcmp(prior,actors,sizeof prior)&&visible==old_visible);
 puts("PASS sourcecourse ground/projection all20 traversal, first actor visible,60tick original4burstframes then inactive, invalid atomicity. Controlled camera, not GBA view oracle.");
 taxi_challenge_ball_course_free(&c);assert(!c.resources.world.storage&&!c.actors.source);
 puts("PASS original Ball510vertices/217polygons/20actors: allsource X/Z/kind/ground/native44byte fields and unwritten retention, invalid atomicity, resource replacement ownership,64 original vehicle starts. Rendering/live Ball not integrated.");return 0;
}
