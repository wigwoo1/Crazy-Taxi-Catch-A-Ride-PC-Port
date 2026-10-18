#include "../src/passenger_scene.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>
static int32_t word(const uint8_t *p) {uint32_t u=p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);int32_t s;memcpy(&s,&u,4);return s;}
typedef struct {unsigned count,events,presentations,bank;} LoadingEvents;
typedef struct {uint32_t *state;unsigned callbacks;} LiveLoading;
static void advance_live(void *context,unsigned pass,size_t index){
    LiveLoading *s=context;(void)pass;(void)index;
    *s->state=*s->state*1664525u+1013904223u;s->callbacks++;
}
static void loading_record(void *context,unsigned pass,size_t index) {
    LoadingEvents *e=context;
    assert(pass==e->events/e->count&&index==e->events%e->count);
    e->events++;
    if(index%5==0){e->presentations++;e->bank=(e->bank+1)%5;}
}
static int selected_before_fare(void *context,const TaxiPassengerScene *scene,int32_t destination) {
    unsigned *count=context;(*count)++;
    assert(destination>=0 && scene->pickup.destination_index==destination);
    assert(!scene->session.trip.active_1694);
    assert(scene->session.fare.pending_whole_16a0==0);
    return 0;
}
static int removed_before_settlement(void *context,const TaxiPassengerScene *scene,int32_t destination) {
    unsigned *calls=context;(*calls)++;
    assert(scene->session.gate_1700==120 || scene->session.trip.time_16ac<=0);
    assert(destination==-1 && scene->pickup.destination_index==-1);
    assert(scene->session.trip.active_1694);
    return 0;
}
static int delivered_before_settlement(void *context,const TaxiPassengerScene *scene,int32_t destination) {
    assert(scene->session.gate_1700==120);
    return removed_before_settlement(context,scene,destination);
}

/* Controlled HUD values from original1EE28; these are state fixtures, not a
   driven trip or a replacement initializer for the production viewer. */
static void display_put(uint8_t *p,uint32_t x,unsigned n){
    for(unsigned i=0;i<n;i++)p[i]=(uint8_t)(x>>(i*8));
}
static void display_fixture(uint8_t scalar[32],uint8_t cache[256],int active){
    memset(scalar,0,32);memset(cache,0,256);scalar[0x14]=99;
    scalar[0x15]=(uint8_t)active;display_put(scalar+0x18,61952,4);
    display_put(cache+0x88,72,2);display_put(cache+0x8a,72,2);
    display_put(cache+0x8e,72,2);display_put(cache+0x90,72,2);
}
static void check_periodic_display_integration(const TaxiTrig *trig){
    for(unsigned test=0;test<9;test++){
        TaxiPassengerScene scene={0};TaxiPassengerAsset asset={0};TaxiWorldAsset world={0};
        taxi_fare_session_init(&scene.session);scene.session.settings.bytes[0]=1;
        scene.session.clock.remaining_16a8=1;scene.session.rank_16d4=99;
        scene.session.end.result_16d0=1;scene.session.end.phase_16d1=7;scene.session.end.result_timer_16d2=2;
        TaxiCashGlyphs cash={0};cash.total_target[0]=cash.pending_target[0]=2;
        uint8_t scalar[32],cache[256];display_fixture(scalar,cache,1);
        TaxiRankDisplayOwner owner={scalar,cache};
        TaxiPassengerFrameInput in={.delta=test==8?2:1,.cash_glyphs=&cash,.rank_display=&owner};
        if(test==1)in.fade=1;
        if(test==2)in.start_active=1;
        if(test==3)in.freeze=1;
        if(test==4)scene.session.ended_1697=1;
        if(test==5)scene.session.outer_state_1ea8=2;
        if(test==6)scene.session.settings.bytes[0]=128;
        if(test==7)scene.session.settings.bytes[0]=33;
        TaxiPassengerFrameEffects effects={0};
        assert(!taxi_passenger_scene_clock_service_observed(&scene,&asset,&world,trig,&in,&effects,NULL,NULL));
        int excluded=test>=1&&test<=6;
        if(excluded){
            assert(cash.total[0]==0&&cash.pending[0]==0);
            assert(word(scalar+0x18)==61952&&scene.session.end.result_timer_16d2==2);
        }else{
            assert(effects.main_expired&&scene.session.ended_1697);
            assert(cash.total[0]==1&&cash.pending[0]==1); /* One call, not delta times. */
            assert(word(scalar+0x18)==(test==7?61952:61952-640*in.delta));
            assert(scene.session.end.result_timer_16d2==(test==8?0:1));
            if(test==8)assert(!scene.session.end.result_16d0&&!scene.session.end.phase_16d1);
            TaxiCashGlyphs old_cash=cash;uint8_t old_scalar[32],old_cache[256];
            memcpy(old_scalar,scalar,32);memcpy(old_cache,cache,256);
            memset(&effects,0,sizeof effects);
            assert(!taxi_passenger_scene_clock_service_observed(&scene,&asset,&world,trig,&in,&effects,NULL,NULL));
            assert(!memcmp(&cash,&old_cash,sizeof cash)&&!memcmp(scalar,old_scalar,32)&&!memcmp(cache,old_cache,256));
        }
    }
    puts("PASS periodic settlement integration: six entry exclusions, newly expired frame, mode gating and variable delta.");
}

int main(int argc,char **argv) {
    if(argc!=4&&!(argc==5&&!strcmp(argv[4],"--warm-reentry-only"))) return 2;
    uint8_t runtime_bytes[34504];
    FILE *runtime_file=fopen(argv[3],"rb");assert(runtime_file);
    assert(fread(runtime_bytes,1,sizeof runtime_bytes,runtime_file)==sizeof runtime_bytes);
    assert(fgetc(runtime_file)==EOF&&!ferror(runtime_file));assert(!fclose(runtime_file));
    assert(!memcmp(runtime_bytes,"CTR1",4));
    TaxiTrig trig;taxi_trig_decode(&trig,runtime_bytes+4,runtime_bytes+8196);
    check_periodic_display_integration(&trig);
    /* Controlled boarding crosses source cull boundary during this update. */
    {
        TaxiPassengerScene edge={.count=1,.candidate=-1};
        uint8_t *p=edge.customers[0].bytes;p[0]=1;p[10]=3;p[9]=5;p[12]=5;
        int32_t x=12799;uint32_t animation=0x087ec4f4;
        memcpy(p+16,&x,4);memcpy(p+28,&animation,4);
        edge.pickup.selected_index=-1;edge.pickup.approach_x_7534=256;
        TaxiPassengerFrameInput input={.delta=1};
        assert(!taxi_passenger_scene_customer_service(&edge,&input));
        assert(word(edge.customers[0].bytes+16)==12800);
        assert(word(edge.customers[0].bytes+36)!=0);
        assert(edge.render_entries_valid && edge.render_eligible[0]);
    }
    TaxiWorldAsset world={0};TaxiPassengerAsset asset={0};TaxiPassengerScene scene;
    assert(!taxi_world_asset_load(&world,argv[1]));assert(!taxi_passenger_asset_load(&asset,argv[2]));
    assert(!taxi_passenger_scene_init(&scene,&asset,&world,0,0));
    assert(scene.marker_phase.clock_5f28==0 && scene.marker_phase.frame_5f29==0 && scene.marker_phase.direction_5f2a==1);
    TaxiPassengerScene observed;LoadingEvents loading={.count=asset.count};
    assert(!taxi_passenger_scene_init_observed(&observed,&asset,&world,0,0,loading_record,&loading));
    assert(loading.events==asset.count*2&&loading.presentations==2*((asset.count+4)/5));
    assert(loading.bank==loading.presentations%5);
    assert(observed.count==scene.count&&observed.eligible==scene.eligible);
    assert(!memcmp(observed.customers,scene.customers,sizeof scene.customers));
    assert(!memcmp(observed.spawns,scene.spawns,sizeof scene.spawns));
    assert(!memcmp(observed.destinations,scene.destinations,sizeof scene.destinations));
    const uint32_t shared_states[]={0,1,0x80000000u,0xffffffffu,123456789};
    for(unsigned i=0;i<sizeof shared_states/sizeof shared_states[0];i++){
        TaxiPassengerScene shared;
        assert(!taxi_passenger_scene_init_shared(&shared,&asset,&world,0,0,shared_states[i],NULL,NULL));
        assert(shared.random_state==shared_states[i]&&shared.use_native_random==1);
        assert(!memcmp(shared.customers,scene.customers,sizeof scene.customers));
        assert(!memcmp(shared.spawns,scene.spawns,sizeof scene.spawns));
        TaxiPassengerScene previous=shared;
        assert(taxi_passenger_scene_init_shared(&shared,NULL,&world,0,0,0,NULL,NULL)==-1);
        assert(!memcmp(&shared,&previous,sizeof shared));
    }
    unsigned eligible=0,changed=0,missing=0;
    {
        TaxiPassengerScene shared=scene;shared.random_state=0xfffffff0u;
        LiveLoading live={.state=&shared.random_state};
        uint32_t expected=shared.random_state;
        for(unsigned i=0;i<asset.count*2;i++)expected=expected*1664525u+1013904223u;
        assert(!taxi_passenger_scene_init_live(&shared,&asset,&world,0,0,&shared.random_state,advance_live,&live));
        assert(live.callbacks==asset.count*2&&shared.random_state==expected&&shared.use_native_random);
        assert(!memcmp(shared.customers,scene.customers,sizeof scene.customers));
        TaxiPassengerScene old=shared;
        assert(taxi_passenger_scene_init_live(&shared,&asset,&world,0,0,NULL,NULL,NULL)==-1);
        assert(!memcmp(&old,&shared,sizeof shared));
        /* Callback generator is a fixture, not claimed original RNG. */
    }
    for(unsigned i=0;i<scene.count;i++) if(scene.customers[i].bytes[0]==1) {
        eligible++;
        const uint8_t *p=scene.customers[i].bytes;
        assert(word(p+16)==word(asset.source[i]+4)&&word(p+24)==word(asset.source[i]+12));
        int32_t height;int status=taxi_world_asset_height(&world,(TaxiPoint){word(p+16),0,word(p+24)},&height);
        assert(status>=0&&word(p+20)==height);
        changed+=(height!=word(asset.source[i]+8));missing+=(status==0);
    }
    assert(eligible==scene.eligible);
    TaxiPassengerScene retained=scene;
    retained.random_state=12345;retained.use_native_random=1;
    for(unsigned i=0;i<retained.count;i++){
        int32_t y=(i&1)?60000:-50000;
        for(unsigned k=0;k<4;k++)retained.customers[i].bytes[20+k]=(uint8_t)((uint32_t)y>>(8*k));
    }
    assert(!taxi_passenger_scene_reinit(&retained,&asset,&world,0));
    assert(retained.random_state==12345 && retained.use_native_random==1);
    for(unsigned i=0;i<retained.count;i++)if(retained.customers[i].bytes[0]==1){
        int32_t h;TaxiPoint point={retained.spawns[i].x,(i&1)?60000:-50000,retained.spawns[i].z};
        assert(taxi_world_asset_height(&world,point,&h)>=0);
        assert(word(retained.customers[i].bytes+20)==h);
    }
    {
        TaxiPassengerScene warm=scene;
        warm.random_state=0xfffffff0u;
        for(unsigned i=0;i<warm.count;i++){
            int32_t y=(i&1)?60000:-50000;
            for(unsigned k=0;k<4;k++)warm.customers[i].bytes[20+k]=(uint8_t)((uint32_t)y>>(8*k));
        }
        TaxiPassengerScene reference=warm;
        assert(!taxi_passenger_scene_reinit(&reference,&asset,&world,0));
        LiveLoading live={.state=&warm.random_state};uint32_t expected=warm.random_state;
        for(unsigned i=0;i<asset.count*2;i++)expected=expected*1664525u+1013904223u;
        assert(!taxi_passenger_scene_reinit_live(&warm,&asset,&world,0,&warm.random_state,advance_live,&live));
        assert(live.callbacks==asset.count*2&&warm.random_state==expected&&warm.use_native_random);
        assert(!memcmp(warm.customers,reference.customers,sizeof warm.customers));
        TaxiPassengerScene before=warm;
        assert(taxi_passenger_scene_reinit_live(&warm,&asset,&world,0,NULL,NULL,NULL)==-1);
        assert(!memcmp(&before,&warm,sizeof warm));
        puts("PASS warm passenger reentry: prior heights retained, both loading passes observed, aliased live RNG advances retained.");
    }
    if(argc==5){taxi_passenger_asset_free(&asset);taxi_world_asset_free(&world);return 0;}
    /* Place the diagnostic taxi at an eligible source spawn. Completion must
       arise through native boarding, not a manually injected flag32. */
    assert(scene.session.clock.remaining_16a8==3000);
    assert(scene.session.distance_display.offset_16c4==-68);
    if(eligible) {
    unsigned chosen=0;while(chosen<scene.count&&scene.customers[chosen].bytes[0]!=1) chosen++;
    assert(chosen<scene.count);
    TaxiRoutePoint taxi=scene.spawns[chosen];
    unsigned selection_calls=0;
    TaxiPassengerFrameInput pickup_frame={.taxi={taxi.x,0,taxi.z},.speed=0,.delta=1};
    TaxiPassengerFrameEffects pickup_effects;
    for(unsigned tick=0;tick<4;tick++) assert(!taxi_passenger_scene_frame_observed(
        &scene,&asset,&world,&trig,&pickup_frame,&pickup_effects,selected_before_fare,&selection_calls));
    assert(selection_calls==1);
    assert(scene.session.distance_display.offset_16c4>-68);

    assert(scene.pickup.pending_1704==0&&scene.pickup.destination_index>=0);
    assert(scene.session.trip.active_1694==1);
    assert(scene.session.trip.active_customer==scene.pickup.selected_index);
    assert(scene.session.trip.initial_time_16b0>0);
    assert(scene.session.fare.pending_whole_16a0>0);
    TaxiPassengerFrameInput frame={.taxi={taxi.x,0,taxi.z},.speed=1,.delta=1};
    TaxiPassengerFrameEffects effects;
    int32_t time=scene.session.trip.time_16ac;
    assert(!taxi_passenger_scene_frame(&scene,&asset,&world,&trig,&frame,&effects));
    assert(scene.session.trip.time_16ac==time-1&&!effects.delivered);
    /* Explicit destination fixture: teleport, query original road material.
       This checks native connected settlement, not an actual driven route. */
    const uint8_t *dest=asset.destinations[scene.pickup.destination_index];
    frame.taxi=(TaxiPoint){word(dest+24),0,word(dest+32)};frame.speed=0;
    TaxiGroundHit ground={0};assert(taxi_world_asset_ground(&world,frame.taxi,&ground)==1);
    frame.material=ground.material;
    /* Controlled destination fixture: pickup must not settle. A speed change
       between the two stages must be observed by delivery, just as traffic
       can change the live taxi state before original B4C6. */
    {
        TaxiPassengerScene split=scene;
        TaxiPassengerFrameEffects split_effect={0};
        unsigned split_calls=0;
        TaxiPassengerScene staged=scene;
        TaxiPassengerFrameEffects staged_effect={0};
        assert(!taxi_passenger_scene_pickup_transition_observed(&staged,&asset,&world,&trig,
            &frame,&staged_effect,NULL,NULL));
        for(unsigned i=0;i<scene.count;i++)
            assert(!memcmp(staged.customers[i].bytes+16,scene.customers[i].bytes+16,12));
        assert(!taxi_passenger_scene_customer_service(&staged,&frame));
        assert(!taxi_passenger_scene_pickup_service_observed(&split,&asset,&world,&trig,
            &frame,&split_effect,NULL,NULL));
        assert(!memcmp(&staged,&split,sizeof split));
        assert(!memcmp(&staged_effect,&split_effect,sizeof split_effect));
        assert(!split_effect.delivered&&split.session.trip.active_1694);
        uint8_t rank_scalar[32],rank_cache[256];display_fixture(rank_scalar,rank_cache,0);
        TaxiRankDisplayOwner rank_owner={rank_scalar,rank_cache};TaxiCashGlyphs display_cash;taxi_cash_glyphs_init(&display_cash);
        TaxiPassengerFrameInput after_traffic=frame;after_traffic.speed=1;
        after_traffic.rank_display=&rank_owner;after_traffic.cash_glyphs=&display_cash;
        TaxiPassengerScene before_delivery=split;
        assert(!taxi_passenger_scene_delivery_service_observed(&split,&asset,&world,&trig,
            &after_traffic,&split_effect,delivered_before_settlement,&split_calls));
        assert(!split_calls&&!split_effect.delivered);
        assert(!rank_scalar[0x15]&&word(rank_scalar+0x18)==61952);
        assert(!memcmp(&before_delivery,&split,sizeof split));
        after_traffic.speed=0;
        assert(!taxi_passenger_scene_delivery_service_observed(&split,&asset,&world,&trig,
            &after_traffic,&split_effect,delivered_before_settlement,&split_calls));
        assert(split_calls==1&&split_effect.delivered&&!split.session.trip.active_1694);
        assert(split.session.gate_1700==120&&split_effect.destination_event_count==1);
        assert(rank_scalar[0x15]==1&&rank_scalar[0x16]==0&&word(rank_scalar+0x18)==61312);
        assert(rank_scalar[0x14]==split.session.rank_16d4);
        assert((rank_cache[0x8e]|(unsigned)rank_cache[0x8f]<<8)==(unsigned)(split.session.rank_16d4/10)*8);
        assert((rank_cache[0x90]|(unsigned)rank_cache[0x91]<<8)==(unsigned)(split.session.rank_16d4%10)*8);
        assert(split.session.end.result_timer_16d2==180);
        taxi_fare_session_cash_targets(&split.session,&display_cash); /* Actual viewer's late target refresh. */
        TaxiPassengerFrameEffects next_display={0};
        assert(!taxi_passenger_scene_clock_service_observed(&split,&asset,&world,&trig,
            &after_traffic,&next_display,NULL,NULL));
        assert(word(rank_scalar+0x18)==60672&&split.session.end.result_timer_16d2==179);
        after_traffic.freeze=1;
        assert(!taxi_passenger_scene_clock_service_observed(&split,&asset,&world,&trig,
            &after_traffic,&next_display,NULL,NULL));
        assert(word(rank_scalar+0x18)==60672&&split.session.end.result_timer_16d2==179);
        puts("PASS accepted-delivery rank trigger and next early periodic update; controlled destination fixture, not an earned route.");
    }
    {unsigned removal_calls=0;
    assert(!taxi_passenger_scene_frame_observed(&scene,&asset,&world,&trig,&frame,&effects,
        delivered_before_settlement,&removal_calls));
    assert(removal_calls==1);}

    assert(effects.delivered&&!scene.session.trip.active_1694);
    assert(effects.destination_event_count==1 && effects.destination_events[0]==-1);
    assert(scene.pickup.destination_index==-1&&scene.session.gate_1700==119);
    assert(scene.session.fare.total_whole_1698>0&&scene.session.fare.pending_whole_16a0==0);
    int32_t delivered_index=scene.pickup.selected_index;
    assert(scene.customers[delivered_index].bytes[0]&64);
    frame.speed=1; /* Prevent a new pickup while observing the exiting actor. */
    for(unsigned tick=0;tick<130;tick++)
        assert(!taxi_passenger_scene_frame(&scene,&asset,&world,&trig,&frame,&effects));
    assert(!(scene.customers[delivered_index].bytes[0]&65));
    assert(!taxi_passenger_scene_init(&scene,&asset,&world,0,0));
    assert(scene.marker_phase.clock_5f28==0 && scene.marker_phase.frame_5f29==0 && scene.marker_phase.direction_5f2a==1);
    scene.use_native_random=1;
    selection_calls=0;
    pickup_frame=(TaxiPassengerFrameInput){.taxi={taxi.x,0,taxi.z},.speed=0,.delta=1};

    for(unsigned tick=0;tick<4;tick++) assert(!taxi_passenger_scene_frame_observed(
        &scene,&asset,&world,&trig,&pickup_frame,&pickup_effects,selected_before_fare,&selection_calls));
    assert(selection_calls==1);

    assert(scene.random_state!=0 && scene.session.trip.active_1694);
    scene.session.trip.time_16ac=1;
    frame.taxi=(TaxiPoint){taxi.x,0,taxi.z};frame.speed=1;
    int32_t allowance=scene.session.trip.allowance_16e0;
    uint32_t random_before_timeout=scene.random_state;
    {unsigned removal_calls=0;
    assert(!taxi_passenger_scene_frame_observed(&scene,&asset,&world,&trig,&frame,&effects,
        removed_before_settlement,&removal_calls));
    assert(removal_calls==1);}

    assert(effects.timed_out&&!effects.delivered&&!scene.session.trip.active_1694);
    assert(effects.destination_event_count==1 && effects.destination_events[0]==-1);
    assert(scene.random_state!=random_before_timeout);
    assert(scene.session.trip.allowance_16e0==allowance);
    assert(scene.session.fare.pending_whole_16a0>0&&scene.session.fare.total_whole_1698==0);
    TaxiPassengerScene before=scene;frame.freeze=1;
    assert(!taxi_passenger_scene_frame(&scene,&asset,&world,&trig,&frame,&effects));
    /* Actual paused22F34 still advances flag64 byte11 before its freeze gate,
       and2318E refreshes visible frame pointers. All other owners stay frozen. */
    TaxiPassengerScene paused_expected=before;paused_expected.render_entries_valid=1;
    for(unsigned i=0;i<paused_expected.count;i++){
        uint8_t *p=paused_expected.customers[i].bytes;
        int64_t dx=(int64_t)frame.taxi.x-word(p+16),dz=(int64_t)frame.taxi.z-word(p+24);
        if(dx<0)dx=-dx;if(dz<0)dz=-dz;
        int nearby=dx<=12799&&dz<=12799;
        int signed_kind=p[1]<128?p[1]:(int)p[1]-256;
        if(nearby&&signed_kind<=7&&(p[0]&64)){
            uint8_t next=(uint8_t)(p[11]+1);
            int signed_next=next<128?next:(int)next-256;
            p[11]=signed_next>89?90:next;
        }
        int visible=nearby&&p[1]<8&&(p[0]&3)==1;
        paused_expected.render_eligible[i]=(uint8_t)visible;
        if(visible)assert(!taxi_passenger_frame_select(paused_expected.customers+i));
    }
    assert(!memcmp(&paused_expected,&scene,sizeof scene));
    frame.freeze=0;frame.delta=4;
    TaxiPassengerScene variable=scene;TaxiPassengerFrameEffects variable_effects={0};
    int32_t prior_remaining=variable.session.clock.remaining_16a8;
    assert(!taxi_passenger_scene_clock_service_observed(&variable,&asset,&world,&trig,&frame,&variable_effects,NULL,NULL));
    assert(variable.session.clock.remaining_16a8==prior_remaining-4);
    frame.delta=61;before=scene;
    memset(&effects,0x5a,sizeof(effects));TaxiPassengerFrameEffects effect_before=effects;
    assert(taxi_passenger_scene_frame(&scene,&asset,&world,&trig,&frame,&effects)==-1);
    assert(!memcmp(&before,&scene,sizeof(scene))&&!memcmp(&effect_before,&effects,sizeof(effects)));
    frame.delta=1;frame.freeze=0;scene.session.clock.remaining_16a8=1;
    assert(!taxi_passenger_scene_frame(&scene,&asset,&world,&trig,&frame,&effects));
    assert(effects.main_expired&&scene.session.ended_1697&&scene.session.expiry.scale_16c0==64);
    before=scene;frame.freeze=1;
    assert(!taxi_passenger_scene_frame(&scene,&asset,&world,&trig,&frame,&effects));
    assert(!memcmp(&before,&scene,sizeof(scene)));
    frame.freeze=0;
    for(unsigned tick=0;tick<167;tick++) {
        assert(!taxi_passenger_scene_frame(&scene,&asset,&world,&trig,&frame,&effects));
        assert(!effects.expiry_fade&&!scene.session.clock.remaining_16a8);
    }
    assert(!taxi_passenger_scene_frame(&scene,&asset,&world,&trig,&frame,&effects));
    assert(effects.expiry_fade&&scene.session.outer_state_1ea8==2&&!scene.session.ended_1697);
    before=scene;
    for(unsigned tick=0;tick<10;tick++) {
        assert(!taxi_passenger_scene_frame(&scene,&asset,&world,&trig,&frame,&effects));
        assert(!effects.expiry_fade&&!effects.main_expired&&!memcmp(&before,&scene,sizeof(scene)));
    }
    }
    if(strstr(argv[1],"city1")) assert(eligible==68);
    if(strstr(argv[1],"city2")) assert(eligible==76);
    printf("Native customer road placement: %u eligible, %u heights differ from source Y, %u missing surfaces; explicit seed Y=0\n",eligible,changed,missing);
    taxi_passenger_asset_free(&asset);taxi_world_asset_free(&world);return 0;
}
