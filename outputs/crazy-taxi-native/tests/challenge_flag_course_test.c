#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "challenge_flag_course.h"
#include "runtime_asset.h"
#include "game_start.h"
#include "challenge_flag_hud.h"
#include "challenge_result_strings.h"
static int32_t word(const uint8_t *p){uint32_t u=p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);int32_t v;memcpy(&v,&u,4);return v;}
int main(void){
 uint8_t result_settings[512]={0};memcpy(result_settings+8,"AAA",3);memcpy(result_settings+216,"BBB",3);
 TaxiChallengeFlagStrings strings;
 for(unsigned profile=0;profile<2;profile++){
  for(unsigned ticks=0;ticks<=1200;ticks++){
   char expected[32];snprintf(expected,sizeof expected,"00:%02u:%02u",ticks/60,(ticks%60)*100/60);
   assert(!taxi_challenge_flag_strings(&strings,result_settings,profile,(int16_t)(1200-ticks),"--"));
   assert(!strcmp(strings.result,expected)&&!strcmp(strings.name,profile?"BBB":"AAA"));
   for(unsigned row=0;row<3;row++)assert(!strcmp(strings.records[row],"--")&&!strcmp(strings.names[row],"--"));
  }
  for(unsigned key=0;key<65536;key++){
   unsigned owner=key&1;uint32_t packed=(key<<15)|(owner<<7)|1;
   for(unsigned byte=0;byte<4;byte++)result_settings[208*profile+120+byte]=(uint8_t)(packed>>(8*byte));
   char expected[32];snprintf(expected,sizeof expected,"%02u:%02u:%02u",key/3600,key/60%60,(key%60)*100/60);
   assert(!taxi_challenge_flag_strings(&strings,result_settings,profile,1199,"--"));
   assert(!strcmp(strings.result,"00:00:01")&&!strcmp(strings.records[0],expected)&&!strcmp(strings.names[0],owner?"BBB":"AAA"));
  }
  memset(result_settings+208*profile+120,0,12);
 }
 assert(!taxi_challenge_flag_strings(&strings,result_settings,0,INT16_MIN,"--")&&!strcmp(strings.result,"09:26:13"));
 assert(!taxi_challenge_flag_strings(&strings,result_settings,0,1201,"--")&&!strcmp(strings.result,"00:00:0/"));
 TaxiChallengeFlagStrings before_strings=strings;
 assert(taxi_challenge_flag_strings(&strings,result_settings,2,1,"--")==-1&&!memcmp(&strings,&before_strings,sizeof strings));
 assert(taxi_challenge_flag_strings(&strings,result_settings,0,1,"long")==-1&&!memcmp(&strings,&before_strings,sizeof strings));
 puts("PASS original Flag results: all1201 elapsed tick values/bothprofiles,131072 packed16bit record keys/owner names/empty rows, signed source boundaries and transactional errors.");
 TaxiFlagHudPlan hud;
 assert(!taxi_challenge_flag_hud_plan(&hud,1200,160,0)&&!strcmp(hud.time,"00'20\"00")&&hud.indicator_y==137&&hud.indicator_variant==1);
 assert(hud.glyphs[0].glyph==16&&hud.glyphs[1].glyph==16&&hud.glyphs[2].glyph==2&&hud.glyphs[4].glyph==18);
 assert(hud.glyphs[0].scaled&&hud.glyphs[1].scaled&&!hud.glyphs[2].scaled&&hud.glyphs[0].position_identity==0x087ec15c&&hud.glyphs[4].position_identity==0x087ec12c);
 assert(!taxi_challenge_flag_hud_plan(&hud,1199,160,32)&&!strcmp(hud.time,"00'19\"98")&&hud.indicator_variant==2);
 assert(!taxi_challenge_flag_hud_plan(&hud,1,320,0)&&!strcmp(hud.time,"00'00\"01")&&hud.indicator_y==297);
 assert(!taxi_challenge_flag_hud_plan(&hud,-1,160,0)&&!strcmp(hud.time,"00'00\"00"));
 assert(!taxi_challenge_flag_hud_plan(&hud,32767,160,0)&&!strcmp(hud.time,"09'06\"11"));
 assert(taxi_challenge_flag_hud_plan(NULL,1200,160,0)==-1);
 puts("PASS original Flag HUD time/glyph request metadata: 20s,19.98s,0.01s,negative clamp, signed16 maximum, scale128 first two glyphs/source positions/indicator variants.");
 uint8_t positions[60];FILE *position_file=fopen("outputs/crazy-taxi-native/assets/challenge-flag/flag-hud-positions.bin","rb");assert(position_file&&fread(positions,1,60,position_file)==60&&fgetc(position_file)==EOF);fclose(position_file);
 assert(!taxi_challenge_flag_hud_positions(&hud,positions,sizeof positions));
 const int32_t xs[5]={56,50,44,32,20},ys[5]={14,14,2,2,2};
 for(unsigned i=0;i<5;i++)assert(hud.glyphs[i].x==xs[i]&&hud.glyphs[i].y==ys[i]&&hud.glyphs[i].z==0);
 TaxiFlagHudPlan before_hud=hud;assert(taxi_challenge_flag_hud_positions(&hud,positions,59)==-1&&!memcmp(&hud,&before_hud,sizeof hud));
 puts("PASS exact original Flag HUD position records decoded in source reverse-glyph order; invalid length retains plan.");
 TaxiFontAsset clock={0};assert(!taxi_font_asset_load(&clock,"outputs/crazy-taxi-native/assets/challenge-flag/flag-clock-metrics.ctf","outputs/crazy-taxi-native/assets/hud/clock.rgba",2,16,0));
 assert(!taxi_challenge_flag_hud_plan(&hud,1199,160,0)&&!taxi_challenge_flag_hud_positions(&hud,positions,60));
 uint8_t hud_pixels[153600];assert(!taxi_challenge_flag_hud_pixels(hud_pixels,&hud,&clock));unsigned visible=0;
 for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++)if(hud_pixels[(y*240+x)*4+3]){visible++;assert(x>=20&&x<60&&y>=2&&y<18);}
 assert(visible);printf("PASS Flag originalgroup3 bank0 HUD glyph raster: %u opaque pixels within recovered source layout, scale128 inverse matrix.\n",visible);taxi_font_asset_free(&clock);
 TaxiChallengeFlagCourse c={0};const char *root="outputs/crazy-taxi-native/assets";
 assert(!taxi_challenge_flag_course_load(&c,root));
 TaxiGroundHit spawn={.height=INT_MIN};TaxiPoint point=c.resources.spawn.query;
 assert(taxi_world_asset_ground(&c.resources.world,point,&spawn)==1);
 int32_t flag_y;assert(taxi_world_asset_height(&c.resources.world,(TaxiPoint){word(c.actors.source[0]+4),INT_MIN,word(c.actors.source[0]+12)},&flag_y)==1&&flag_y==735);
 for(unsigned i=0;i<c.resources.textures.count;i++){TaxiIndexedTexture t;assert(!taxi_indexed_asset_texture(&c.resources.textures,i,&t)&&t.pixels&&t.size==t.width*t.height);}
 TaxiRuntimeAsset runtime;assert(!taxi_runtime_asset_load(&runtime,"outputs/crazy-taxi-native/assets/driving.ctr"));
 for(unsigned bank=0;bank<2;bank++)for(unsigned options=0;options<32;options++){
  TaxiDriveWorld vehicle;TaxiDriveWorldContext context;
  assert(taxi_challenge_flag_course_start(&c,&vehicle,&context,runtime.templates[bank],584,(uint8_t)(128|options))==1);
  assert(vehicle.world.height.position.x==point.x&&vehicle.world.height.position.z==point.z&&vehicle.world.height.position.y==spawn.height+64);
  assert(context.driver.motion.orientation.submode==1&&context.driver.motion.orientation.settings==(128|options));
  assert(vehicle.driver.power.gears.gear==0&&vehicle.initial_camera_mode_64c==2&&vehicle.initial_camera_heading_654==(int32_t)c.resources.spawn.heading*4096);
  TaxiDriveWorld before_vehicle=vehicle;TaxiDriveWorldContext before_context=context;
  assert(taxi_challenge_flag_course_start(&c,&vehicle,&context,runtime.templates[bank],584,0)==-1&&!memcmp(&vehicle,&before_vehicle,sizeof vehicle)&&!memcmp(&context,&before_context,sizeof context));
 }
 int32_t hints[3]={INT_MIN,0,707};
 for(unsigned hint=0;hint<3;hint++){
  TaxiPassengerState flag;memset(&flag,0xa5,sizeof flag);
  uint32_t y;memcpy(&y,hints+hint,4);for(unsigned i=0;i<4;i++)flag.bytes[20+i]=(uint8_t)(y>>(8*i));
  assert(taxi_challenge_flag_course_actor(&c,&flag)==1);
  assert(flag.bytes[0]==1&&flag.bytes[1]==0&&word(flag.bytes+16)==87770&&word(flag.bytes+20)==735&&word(flag.bytes+24)==-3);
  assert((uint32_t)word(flag.bytes+28)==0x087ec47c&&flag.bytes[32]==32&&flag.bytes[33]==64&&flag.bytes[34]==5&&flag.bytes[35]==6);
  for(unsigned i=2;i<8;i++)assert(flag.bytes[i]==0xa5);assert(flag.bytes[11]==0xa5);
  for(unsigned i=36;i<44;i++)assert(flag.bytes[i]==0xa5);
  TaxiPassengerState before_flag=flag;c.resources.submode=0;assert(taxi_challenge_flag_course_actor(&c,&flag)==-1&&!memcmp(&flag,&before_flag,sizeof flag));c.resources.submode=1;
 }
 TaxiDriveWorld vehicle;TaxiDriveWorldContext context;
 assert(taxi_challenge_flag_course_start(&c,&vehicle,&context,runtime.templates[0],584,128)==1);
 TaxiPassengerState flag={0};assert(taxi_challenge_flag_course_actor(&c,&flag)==1);
 TaxiChallengeFlag timer;taxi_challenge_flag_begin(&timer);TaxiChallengeFlagStatus status={0};TaxiChallengeFlagEffects effects;
 TaxiChallengeFlagInput gates={.delta=1,.effects_enabled=1};unsigned warnings=0;
 for(unsigned tick=0;tick<1200;tick++){assert(!taxi_challenge_flag_course_control(&c,&flag,&vehicle,&timer,&gates,&status,&effects));warnings+=effects.warning_palette;assert(!effects.request_rank);}
 assert(timer.remaining_1552==0&&status.ended1697&&!status.success153c&&warnings==1&&flag.bytes[0]==1);
 /* Contact position is a diagnostic fixture, not a driven ROM trajectory. */
 vehicle.world.height.position.x=word(flag.bytes+16);vehicle.world.height.position.z=word(flag.bytes+24);
 taxi_challenge_flag_begin(&timer);status=(TaxiChallengeFlagStatus){0};gates.intro1696=1;
 assert(!taxi_challenge_flag_course_control(&c,&flag,&vehicle,&timer,&gates,&status,&effects));
 assert(timer.remaining_1552==1200&&timer.scale_1554==255&&status.success153c&&status.ended1697&&effects.request_rank&&effects.sound_index==42);
 for(unsigned tick=0;tick<239;tick++){assert(!taxi_challenge_flag_course_control(&c,&flag,&vehicle,&timer,&gates,&status,&effects));assert(!effects.request_rank);}
 assert(timer.scale_1554==16&&effects.hide_flag&&(flag.bytes[0]&3)==2);
 /* Scripted forward driving: original course/start/physics, no teleport. */
 assert(taxi_challenge_flag_course_start(&c,&vehicle,&context,runtime.templates[0],584,128)==1);
 memset(&flag,0,sizeof flag);assert(taxi_challenge_flag_course_actor(&c,&flag)==1);
 taxi_challenge_flag_begin(&timer);status=(TaxiChallengeFlagStatus){0};
 TaxiGameStart start;taxi_game_start_begin_challenge(&start,240);unsigned intro_ticks=0;
 while(start.active&&intro_ticks<1000){
  gates.intro1696=1;
  assert(!taxi_challenge_flag_course_control(&c,&flag,&vehicle,&timer,&gates,&status,&effects));
  taxi_game_start_challenge_step(&start,1,240,0,0,0);
  TaxiGearInput input={.delta=1,.settings=128};TaxiDriveWorldEffects driven;
  assert(!taxi_drive_world_start_step(&vehicle,&context,&input,0,&runtime.trig,&runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&c.resources.world,&driven));
  intro_ticks++;
 }
 assert(intro_ticks==170&&timer.remaining_1552==1200&&!status.ended1697);
 gates.intro1696=0;unsigned movement_ticks=0,drive_ticks=0,ranks=0;warnings=0;
 TaxiPoint initial_position=vehicle.world.height.position;
 while(!status.ended1697&&drive_ticks<1201){
  assert(!taxi_challenge_flag_course_control(&c,&flag,&vehicle,&timer,&gates,&status,&effects));
  warnings+=effects.warning_palette;ranks+=effects.request_rank;
  context.height.flag_1697=status.ended1697;context.height.flag_153c=status.success153c;
  TaxiCommand command={0};command.bytes[4]=4;
  TaxiGearInput input={.delta=1,.frame_delta=1,.settings=128,.action2=1,.action4=(uint8_t)(drive_ticks==0),.command_flags=4};
  TaxiDriveWorldEffects driven;TaxiPoint previous=vehicle.world.height.position;
  assert(!taxi_drive_world_step(&vehicle,&context,&command,&input,0,&runtime.trig,&runtime.traction,&runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&c.resources.world,&driven));
  assert(!driven.world.height.completion_call);
  movement_ticks+=(vehicle.world.height.position.x!=previous.x||vehicle.world.height.position.z!=previous.z);drive_ticks++;
 }
 assert(status.ended1697&&movement_ticks>0);
 assert(vehicle.world.height.position.x!=initial_position.x||vehicle.world.height.position.z!=initial_position.z);
 assert(status.success153c?(ranks==1&&timer.caught_1550):(drive_ticks==1200&&timer.remaining_1552==0&&warnings==1&&ranks==0));
 printf("FLAG FORWARD PROBE intro%u ticks%u movement%u position%ld,%ld,%ld remaining%d success%u rankrequests%u; scripted controls, not ROM trajectory oracle.\n",intro_ticks,drive_ticks,movement_ticks,(long)vehicle.world.height.position.x,(long)vehicle.world.height.position.y,(long)vehicle.world.height.position.z,timer.remaining_1552,status.success153c,ranks);
 uint8_t indicator[1440];FILE *indicator_file=fopen("outputs/crazy-taxi-native/assets/challenge-flag/flag-indicator.indices","rb");assert(indicator_file&&fread(indicator,1,1440,indicator_file)==1440&&fgetc(indicator_file)==EOF);fclose(indicator_file);
 for(unsigned variant=1;variant<=2;variant++){
  assert(!taxi_challenge_flag_hud_plan(&hud,1200,160,(uint8_t)(variant==2?32:0)));memset(hud_pixels,0x5a,sizeof hud_pixels);
  assert(!taxi_challenge_flag_indicator_pixels(hud_pixels,&hud,indicator,sizeof indicator,c.resources.palette));
  unsigned zeros=0;
  for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++){
   uint8_t *pixel=hud_pixels+(y*240+x)*4;
   if(y>=137&&x>=4&&x<24){unsigned index=indicator[variant*480+(y-137)*20+x-4];uint16_t color=c.resources.palette[index];zeros+=!index;for(unsigned channel=0;channel<3;channel++)assert(pixel[channel]==((color>>(channel*5))&31)*255/31);assert(pixel[3]==255);}
   else for(unsigned channel=0;channel<4;channel++)assert(pixel[channel]==0x5a);
  }
  (void)zeros;
  /* Controlled zero index: source frames need not contain one in visible rows. */
  uint8_t zero_fixture[1440];memcpy(zero_fixture,indicator,sizeof zero_fixture);zero_fixture[variant*480]=0;
  assert(!taxi_challenge_flag_indicator_pixels(hud_pixels,&hud,zero_fixture,sizeof zero_fixture,c.resources.palette));
  uint8_t *zero_pixel=hud_pixels+(137*240+4)*4;
  for(unsigned channel=0;channel<3;channel++)assert(zero_pixel[channel]==((c.resources.palette[0]>>(channel*5))&31)*255/31);
  assert(zero_pixel[3]==255);
  TaxiFlagHudPlan invalid=hud;invalid.indicator_variant=0;uint8_t saved[153600];memcpy(saved,hud_pixels,sizeof saved);
  assert(taxi_challenge_flag_indicator_pixels(hud_pixels,&invalid,indicator,1440,c.resources.palette)==-1&&!memcmp(saved,hud_pixels,sizeof saved));
 }
 puts("PASS original Flag indicator variants1/2:460 opaque pixels each at4,137; source rowstride/bottom clip/index0 opaque/palette and surrounding pixels retained.");
 TaxiChallengeFlagCourse before=c;assert(taxi_challenge_flag_course_load(&c,"outputs/crazy-taxi-native/assets/missing-course")==-1&&!memcmp(&c,&before,sizeof c));
 char oversized[600];memset(oversized,'x',599);oversized[599]=0;assert(taxi_challenge_flag_course_load(&c,oversized)==-1&&!memcmp(&c,&before,sizeof c));
 assert(!taxi_challenge_flag_course_load(&c,root));assert(taxi_world_asset_ground(&c.resources.world,point,&spawn)==1);
 printf("PASS original Flag course resource ownership and64vehicle initializations:403 vertices/225 polygons/75 materials/45 indexed textures/one actor; original GRID spawn(%ld,%ld) ground%ld and flagground735; failed replacement retained, successful replacement and free. Flag actor ground/init with three caller-owned Y hints and unwritten byte retention verified; 1200-update timeout/warning and diagnostic contact/single rank request/240-update hide connected; shared session/camera/results pending.\n",(long)point.x,(long)point.z,(long)spawn.height);
 taxi_challenge_flag_course_free(&c);assert(!c.resources.world.storage&&!c.resources.textures.storage&&!c.actors.source);return 0;
}
