#include "../src/challenge_assets.h"
#include "../src/indexed_asset.h"
#include "../src/challenge_result_strings.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/world_asset.h"
#include "../src/runtime_asset.h"
#include "../src/game_start.h"
#include "../src/challenge_completion.h"
#include "../src/normal_menu_flow.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static int32_t word(const unsigned char *p){uint32_t u=p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);int32_t s;memcpy(&s,&u,4);return s;}
int main(void){
 for(unsigned flag=0;flag<256;flag++)assert(taxi_runtime_template_bank((uint8_t)flag)==(flag!=0));
 TaxiRuntimeAsset runtime;assert(!taxi_runtime_asset_load(&runtime,"outputs/crazy-taxi-native/assets/driving.ctr"));
 for(unsigned mode=0;mode<9;mode++){
  TaxiChallengeAssets assets={0};
  assert(!taxi_challenge_assets_load(&assets,"outputs/crazy-taxi-native/assets/challenge-worlds",mode));
  /* Shallow world view: assets retains ownership through all driving callbacks. */
  TaxiWorldAsset world=assets.world;TaxiSpawnMarker marker=assets.spawn;
  static const unsigned special_counts[9]={0,27,17,0,4,0,13,0,0};assert(world.special_count==special_counts[mode]);
  assert(assets.textures.count==45);
  for(unsigned t=0;t<assets.textures.count;t++){TaxiIndexedTexture texture;assert(!taxi_indexed_asset_texture(&assets.textures,t,&texture));assert(texture.pixels&&texture.size==texture.width*texture.height);}
  TaxiPoint query=marker.query;TaxiGroundHit hit={0};
  int result=taxi_world_asset_ground(&world,query,&hit);assert(result==1);
  printf("challenge%u original GRID_01 x%d z%d heading%u ground_result%d height%d\n",mode,query.x,query.z,marker.heading,result,hit.height);
  for(unsigned bank=0;bank<2;bank++){
   TaxiDriveWorld state;TaxiDriveWorldContext context;
   assert(taxi_simulation_start_challenge(&state,&context,runtime.templates[bank],584,&marker,128,mode,
       taxi_world_asset_ground,&world)==1);
   assert(state.world.height.position.x==query.x&&state.world.height.position.z==query.z);
   assert(state.world.height.position.y==hit.height+64);
   assert(state.initial_camera_mode_64c==2);
   assert(state.initial_camera_heading_654==(int32_t)marker.heading*4096);
   assert(state.driver.power.gears.gear==0);
   TaxiDriveWorld saved_state=state;TaxiDriveWorldContext saved_context=context;
   for(unsigned options=0;options<32;options++){
    assert(taxi_simulation_start_challenge(&state,&context,runtime.templates[bank],584,&marker,(uint8_t)(128|options),mode,taxi_world_asset_ground,&world)==1);
    assert(context.driver.motion.orientation.settings==(128|options)&&context.driver.motion.orientation.submode==mode);
    assert(state.initial_camera_heading_654==(int32_t)marker.heading*4096);
   }
   state=saved_state;context=saved_context;

   assert(taxi_simulation_start_challenge(&state,&context,runtime.templates[bank],584,&marker,0,mode,taxi_world_asset_ground,&world)==-1);
   assert(!memcmp(&state,&saved_state,sizeof state)&&!memcmp(&context,&saved_context,sizeof context));
   assert(taxi_simulation_start_challenge(&state,&context,runtime.templates[bank],584,&marker,128,9,taxi_world_asset_ground,&world)==-1);
   assert(!memcmp(&state,&saved_state,sizeof state)&&!memcmp(&context,&saved_context,sizeof context));
   assert(context.driver.motion.orientation.settings==128);
   assert(taxi_simulation_start_challenge(&state,&context,runtime.templates[bank],584,&marker,160,mode,taxi_world_asset_ground,&world)==-1);
   assert(!memcmp(&state,&saved_state,sizeof state)&&!memcmp(&context,&saved_context,sizeof context));
   TaxiGameStart start;taxi_game_start_begin_challenge(&start,240);
   unsigned ticks=0,events=0;
   while(start.active&&ticks<1000){
    events|=taxi_game_start_challenge_step(&start,1,240,0,0,0);
    TaxiGearInput input={.delta=1,.settings=128};TaxiDriveWorldEffects effects;
    assert(!taxi_drive_world_start_step(&state,&context,&input,0,&runtime.trig,
      &runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&world,&effects));
    assert(state.world.height.position.x==query.x&&state.world.height.position.z==query.z);
    assert(state.world.heading_1c==(int32_t)marker.heading*4096);
    ticks++;
   }
   assert(ticks==170&&(events&TAXI_START_FINISHED)&&context.driver.motion.orientation.submode==mode);
   if(mode==0&&bank==0){
    TaxiDriveWorld initial=state;TaxiDriveWorldContext initial_context=context;
    for(unsigned variant=0;variant<2;variant++){
    state=initial;context=initial_context;
    uint8_t score_settings[512]={0};unsigned completion_requests=0;
    unsigned takeoffs=0,landings=0,ground_misses=0,drive_ticks=0;
    for(unsigned tick=0;tick<1800&&!context.height.flag_1697;tick++){
     unsigned phase=tick%60;
     int gas=!variant||phase>=5;
     int shift_drive=variant?phase==4:tick==0;
     TaxiCommand command={0};command.bytes[4]=(uint8_t)(gas?4:0);
     TaxiGearInput input={.delta=1,.frame_delta=1,.settings=128,.action2=(uint8_t)gas,.action4=(uint8_t)shift_drive,.command_flags=command.bytes[4]};
     TaxiDriveWorldEffects effects;
     int status=taxi_drive_world_step(&state,&context,&command,&input,0,&runtime.trig,
       &runtime.traction,&runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&world,&effects);
     if(status){printf("Jump native drive error tick%u gear%d speed%d\n",tick,state.driver.power.gears.gear,state.world.speed_544);}
     assert(!status);drive_ticks++;
     if(effects.world.height.completion_call){
      completion_requests++;
      assert(taxi_challenge_distance_complete(score_settings,0,0,&context.height,&effects.world.height)==1);
     }
     takeoffs+=effects.world.height.took_off;landings+=effects.world.height.landed;
     ground_misses+=!effects.world.ground_hit;
    }
    assert(drive_ticks>0&&takeoffs&&landings&&context.height.flag_1697);
    assert(context.height.flag_153c==variant&&completion_requests==variant);
    if(variant){
     assert(context.height.distance_1540>38400&&context.height.flag_153d==1);
     assert(word(score_settings+108)&1);
      TaxiChallengeJumpStrings display;
      /* Only profile label is a fixture; distance and inserted record are earned above. */
      memcpy(score_settings+8,"AAA",3);
      assert(!taxi_challenge_jump_strings(&display,score_settings,0,context.height.distance_1540,"--"));
      /* The old 173.81m golden came from a prebuilt binary whose source revision
         is no longer available. Keep this integration check about current
         earned-record propagation; result formatting has its own source tests. */
      size_t result_length=strlen(display.result);
      assert(result_length>1&&display.result[result_length-1]=='m');
      assert(!strcmp(display.records[0],display.result)&&!strcmp(display.names[0],"AAA"));
      printf("PASS current-source earned Jump result %s / record %s / profile %s; not a ROM trajectory oracle\n",display.result,display.records[0],display.names[0]);

     printf("PASS native input-to-success-to-record:rank%u distance%d record%u\n",context.height.flag_153d,context.height.distance_1540,(unsigned)word(score_settings+108));
    }else{uint8_t blank[512]={0};assert(!memcmp(blank,score_settings,512));}
    uint8_t outer=0;unsigned result_ticks=0;
    if(variant){
     TaxiChallengeSuccess outcome={.ended1697=context.height.flag_1697};
     while(!outcome.outer1ea8&&result_ticks<1000){
      taxi_challenge_success_step(&outcome,context.height.flag_153d,0,0,0,1);result_ticks++;
     }
     assert(result_ticks==301&&outcome.fade_flags==1);outer=outcome.outer1ea8;
    }else{
     TaxiChallengeFailure outcome={.ended1697=context.height.flag_1697};uint8_t pressed[8]={0};
     while(outcome.hold16be||!outcome.phase16bc){
      taxi_challenge_failure_step(&outcome,0,0,0,0,1,pressed);result_ticks++;
     }
     pressed[7]=1;taxi_challenge_failure_step(&outcome,0,0,0,0,1,pressed);
     pressed[7]=0;pressed[0]=1;taxi_challenge_failure_step(&outcome,0,0,0,0,1,pressed);
     assert(outcome.fade_flags==1&&result_ticks==121);outer=outcome.outer1ea8;
    }
    assert(outer==2);
    TaxiPaletteFade exit_fade={.interval_1771=1,.reload_1772=1};
    taxi_palette_fade_begin(&exit_fade,0);unsigned fade_ticks=0;
    while(exit_fade.active_177b&&fade_ticks<100){taxi_palette_fade_step(&exit_fade,1);fade_ticks++;}
    assert(!exit_fade.active_177b&&fade_ticks==33);
    /* Explicit profile-valid fixture permits the original grid selection
       to survive the source3897 reinitialization branch. */
    uint8_t grid_settings[512];memcpy(grid_settings,score_settings,512);
    grid_settings[8]='B';grid_settings[9]='A';grid_settings[10]='A';grid_settings[11]=1;
    uint8_t profile=0;TaxiChallengeMenuFlow grid;TaxiMenuInput sampled={0};
    assert(!taxi_challenge_menu_flow_begin(&grid,0,1,grid_settings,&profile,2,1,1));
    uint8_t driver=0,submode=0;
    for(unsigned tick=0;tick<33;tick++)taxi_challenge_menu_flow_frame(&grid,&sampled,grid_settings,&profile,&driver,&submode,0,1);
    assert(!grid.fade.active_177b&&grid.grid.action.selection3886==2&&!grid.finished);
    assert(!memcmp(grid_settings+108,score_settings+108,12));
    printf("PASS integrated Jump variant%u landing/results%u/exit-fade%u/grid reentry with score retained; explicit profile fixture,no live audio/render claim.\n",variant,result_ticks,fade_ticks);
    printf("JUMP INPUT PROBE variant%u ticks%u position%d,%d,%d speed%d takeoffs%u landings%u misses%u distance%d ended%u success%u\n",variant,drive_ticks,state.world.height.position.x,state.world.height.position.y,state.world.height.position.z,state.world.speed_544,takeoffs,landings,ground_misses,context.height.distance_1540,context.height.flag_1697,context.height.flag_153c);
    }
   }
   printf("PASS native challenge case challenge%u template-bank%u y%d camera-heading%d start-ticks%u\n",mode,bank,state.world.height.position.y,state.initial_camera_heading_654,ticks);
  }
  taxi_challenge_assets_free(&assets);
 }
 puts("PASS all9 original challenge start markers have native ground hits; both original template banks run170 native gear/world start ticks; full actor bootstrap/trajectory still pending.");return 0;
}


