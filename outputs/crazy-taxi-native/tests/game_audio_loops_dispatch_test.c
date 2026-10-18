#ifdef NDEBUG
#undef NDEBUG
#endif
#include "game_audio_vehicle_loop.h"
#include "game_audio_turn_loop.h"
#include "game_audio_channels.h"
#include <assert.h>
#include <stdio.h>
typedef struct {TaxiGameAudioEngineRequest q[4];unsigned n;uint32_t start;uint8_t eb,lb;} Context;
static uint32_t fixed(void *v,TaxiGameAudioEngineRequest q){Context *c=v;assert(c->n<4);c->q[c->n++]=q;return q.op==1?c->start:q.op==6?(q.value==44?c->eb:c->lb):0;}
static void equal(TaxiGameAudioEngineRequest *q,unsigned n,Context *c){assert(n==c->n);for(unsigned j=0;j<n;j++)assert(q[j].op==c->q[j].op&&q[j].handle==c->q[j].handle&&q[j].value==c->q[j].value);}
int main(int argc,char **argv){assert(argc==2);unsigned cases=0;
 for(unsigned mask=0;mask<512;mask++){
 uint32_t a=mask&1?1:0,b=a;TaxiGameAudioVehicleLoopInput i={(int32_t)(mask&2),(uint8_t)(mask&4?8:0),(uint8_t)(mask&8),(uint8_t)(mask&16),(uint8_t)(mask&32),(uint8_t)(mask&64),mask&128?0:3};Context c={0};c.start=i.start;c.lb=i.busy;TaxiGameAudioEngineRequest q[4];unsigned n=taxi_game_audio_vehicle_loop(&a,&i,q);assert(taxi_game_audio_vehicle_loop_dispatch(&b,&i,fixed,&c)==n&&a==b);equal(q,n,&c);cases++;
 TaxiGameAudioTurnLoop s={mask&1?1:0,mask&2?2:0},t=s;TaxiGameAudioTurnLoopInput ti={(int16_t)(mask&4?-769:768),(uint8_t)(mask&8),(uint8_t)(mask&16?8:0),(uint8_t)(mask&32),0,0,(uint8_t)(mask&64),(uint8_t)(mask&128),mask&256?0:3};c=(Context){0};c.start=ti.start;c.eb=ti.event_busy;c.lb=ti.loop_busy;n=taxi_game_audio_turn_loop(&s,&ti,q);assert(taxi_game_audio_turn_loop_dispatch(&t,&ti,fixed,&c)==n&&s.event_handle==t.event_handle&&s.loop_handle==t.loop_handle);equal(q,n,&c);cases++;
 }
 TaxiAudioAsset assets={0};assert(!taxi_audio_asset_load(&assets,argv[1]));TaxiGameAudioChannels bank={0};bank.assets=&assets;bank.output_rate=10512;uint32_t h=0;TaxiGameAudioVehicleLoopInput vi={0};assert(taxi_game_audio_vehicle_loop_dispatch(&h,&vi,taxi_game_audio_channel_dispatch,&bank)==2&&h==1&&!bank.error);vi.vehicle564=1;assert(taxi_game_audio_vehicle_loop_dispatch(&h,&vi,taxi_game_audio_channel_dispatch,&bank)==2&&!h&&!bank.channels[0].sound);
 /* Mismatched stale vehicle retains; turning loop clears without stopping. */
 assert(taxi_game_audio_channel_dispatch(&bank,(TaxiGameAudioEngineRequest){1,44,0})==1);h=1;assert(taxi_game_audio_vehicle_loop_dispatch(&h,&vi,taxi_game_audio_channel_dispatch,&bank)==1&&h==1);
 TaxiGameAudioTurnLoop turn={1,1};TaxiGameAudioTurnLoopInput ti={0};assert(taxi_game_audio_turn_loop_dispatch(&turn,&ti,taxi_game_audio_channel_dispatch,&bank)==2&&turn.event_handle==1&&!turn.loop_handle&&bank.channels[0].sound==assets.sounds+44&&!bank.error);
 taxi_audio_asset_free(&assets);printf("PASS %u callback equivalence cases plus actual loop lifecycle and stale owner differences\n",cases);return 0;}
