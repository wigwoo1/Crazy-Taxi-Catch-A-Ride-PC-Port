#include "passenger_scene.h"
#include "passenger_search.h"
#include "destination.h"
#include "passenger_delivery.h"
#include "challenge_delivery.h"
#include "passenger_idle.h"
#include "random.h"
#include <string.h>
static FILE *money_trace;
void taxi_passenger_money_trace(FILE *sink){money_trace=sink;}
static void trace_money_state(FILE *sink,const TaxiFareState *p){
    fprintf(sink," %ld %ld %ld %ld %u %u",(long)p->total_whole_1698,
        (long)p->total_fraction_169c,(long)p->pending_whole_16a0,
        (long)p->pending_fraction_16a4,(unsigned)p->rating_1693,(unsigned)p->qualified_1691);
}
static FILE *fare_trace;
void taxi_passenger_fare_trace(FILE *sink){fare_trace=sink;}
static void trace_trip_state(FILE *sink,const TaxiTripStartState *p){
    fprintf(sink," %ld %ld %ld %ld %ld %u %u %u %u %u %u",
        (long)p->active_customer,(long)p->direction_753c,(long)p->time_16ac,
        (long)p->initial_time_16b0,(long)p->allowance_16e0,
        (unsigned)p->scale_5310,(unsigned)p->scale_5312,(unsigned)p->phase_5314,
        (unsigned)p->active_1694,(unsigned)p->active_1695,(unsigned)p->voice_25d6);
}
static FILE *pickup_trace;
void taxi_passenger_pickup_trace(FILE *sink){pickup_trace=sink;}
static void trace_pickup_state(FILE *sink,const TaxiPickupState *p){
    fprintf(sink," %ld %ld %ld %ld %ld %ld %u %u",(long)p->pending_1704,
        (long)p->selected_index,(long)p->dialogue_index,(long)p->destination_index,
        (long)p->approach_x_7534,(long)p->approach_z_7538,
        (unsigned)p->animation_25d7,(unsigned)p->voice_25d6);
}
static int32_t word(const uint8_t *p) {uint32_t u=p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);int32_t s;memcpy(&s,&u,4);return s;}
static int init(TaxiPassengerScene *out,const TaxiPassengerAsset *a,TaxiWorldAsset *world,
    int32_t endpoint_y,int32_t prior_runtime_y,const TaxiPassengerScene *previous,
    TaxiPassengerInitObserver observer,void *context,const TaxiFareSession *prepared) {
    if(!out||!a||!world||a->count>128) return -1;
    /* Original B79E..B7A8 resets clock/frame0, direction1. */
    TaxiPassengerScene s={.marker_phase={.direction_5f2a=1},.count=a->count,.candidate=-1,.pickup={.selected_index=-1,.dialogue_index=-1,.destination_index=-1}};
    if(previous){
        if(previous->count!=a->count)return -1;
        memcpy(s.customers,previous->customers,sizeof(s.customers));
        s.random_state=previous->random_state;s.use_native_random=previous->use_native_random;
    }
    if(prepared)s.session=*prepared;else taxi_fare_session_init(&s.session);
    TaxiPassengerResolved r[128]={0};TaxiPassengerInitEffect effects[128];
    for(unsigned i=0;i<a->count;i++) {
        const uint8_t *p=a->source[i];r[i].kind=p[3];s.destinations[i]=p[1];
        s.spawns[i]=r[i].origin=(TaxiRoutePoint){word(p+4),word(p+12)};
        if(p[3]>9) continue;
        const uint8_t *d=a->destinations[p[1]];r[i].destination=(TaxiRoutePoint){word(d+24),word(d+32)};
        for(unsigned j=0;j<a->route_count;j++) {
            const TaxiPassengerRouteAsset *route=a->routes+j;
            if(route->points[0].x!=r[i].origin.x||route->points[0].z!=r[i].origin.z) continue;
            TaxiRoutePoint end=route->points[route->count-1];TaxiGroundHit hit={0};
            int status=taxi_world_asset_ground(world,(TaxiPoint){end.x,endpoint_y,end.z},&hit);
            if(status<0) return -1;
            if(status&&taxi_destination_accepts(p[1],d,hit.material)) {
                r[i].route=route->points;r[i].point_count=route->count;s.eligible++;break;
            }
        }
    }
    for(unsigned i=0;i<a->count;i++) if(r[i].route) {
        int32_t prior_y=previous?word(previous->customers[i].bytes+20):prior_runtime_y;
        TaxiPoint point={r[i].origin.x,prior_y,r[i].origin.z};
        if(taxi_world_asset_height(world,point,&r[i].spawn_height)<0) return -1;
        /* Original writes INTMIN if no surface, without source-Y fallback. */
    }
    if(taxi_passengers_init_observed(s.customers,r,a->count,effects,observer,context)) return -1;
    *out=s;return 0;
}
int taxi_passenger_scene_init(TaxiPassengerScene *out,const TaxiPassengerAsset *a,
    TaxiWorldAsset *world,int32_t endpoint_y,int32_t prior_runtime_y){
    return init(out,a,world,endpoint_y,prior_runtime_y,0,NULL,NULL,NULL);
}
int taxi_passenger_scene_init_observed(TaxiPassengerScene *out,const TaxiPassengerAsset *a,
    TaxiWorldAsset *world,int32_t endpoint_y,int32_t prior_runtime_y,
    TaxiPassengerInitObserver observer,void *context) {
    return init(out,a,world,endpoint_y,prior_runtime_y,0,observer,context,NULL);
}
int taxi_passenger_scene_reinit(TaxiPassengerScene *out,const TaxiPassengerAsset *a,
    TaxiWorldAsset *world,int32_t endpoint_y){
    if(!out)return -1;
    TaxiPassengerScene previous=*out;
    return init(out,a,world,endpoint_y,0,&previous,NULL,NULL,NULL);
}
int taxi_passenger_scene_init_shared(TaxiPassengerScene *out,const TaxiPassengerAsset *a,
    TaxiWorldAsset *world,int32_t endpoint_y,int32_t prior_runtime_y,uint32_t random_state,
    TaxiPassengerInitObserver observer,void *context){
    TaxiPassengerScene result;
    if(!out||init(&result,a,world,endpoint_y,prior_runtime_y,NULL,observer,context,NULL))return -1;
    result.random_state=random_state;result.use_native_random=1;
    *out=result;return 0;
}
int taxi_passenger_scene_init_live(TaxiPassengerScene *out,const TaxiPassengerAsset *a,
    TaxiWorldAsset *world,int32_t endpoint_y,int32_t prior_runtime_y,const uint32_t *random_state,
    TaxiPassengerInitObserver observer,void *context){
    TaxiPassengerScene result;
    if(!out||!random_state||init(&result,a,world,endpoint_y,prior_runtime_y,NULL,observer,context,NULL))return -1;
    result.random_state=*random_state;result.use_native_random=1;
    *out=result;return 0;
}
int taxi_passenger_scene_reinit_live(TaxiPassengerScene *out,const TaxiPassengerAsset *a,
    TaxiWorldAsset *world,int32_t endpoint_y,const uint32_t *random_state,
    TaxiPassengerInitObserver observer,void *context){
    if(!out||!random_state)return -1;
    TaxiPassengerScene previous=*out,result;
    if(init(&result,a,world,endpoint_y,0,&previous,observer,context,NULL))return -1;
    result.random_state=*random_state;result.use_native_random=1;
    *out=result;return 0;
}
int taxi_passenger_scene_init_prepared_live(TaxiPassengerScene *out,const TaxiPassengerAsset *a,
    TaxiWorldAsset *world,int32_t endpoint_y,int32_t prior_runtime_y,uint8_t warm,
    const TaxiFareSession *prepared,const uint32_t *random_state,TaxiPassengerInitObserver observer,void *context){
    if(!out||!prepared||!random_state)return -1;
    TaxiPassengerScene previous,result;
    if(warm)previous=*out;
    if(init(&result,a,world,endpoint_y,prior_runtime_y,warm?&previous:NULL,observer,context,prepared))return -1;
    result.random_state=*random_state;result.use_native_random=1;
    *out=result;return 0;
}
static int pickup_transition(TaxiPassengerScene *s,TaxiRoutePoint taxi,int32_t speed,int32_t random,TaxiPickupEffects *reported,TaxiPassengerSelectionObserver observer,void *context) {
    int32_t distance;TaxiPickupEffects effect;
    if(taxi_passenger_search(s->customers,s->spawns,s->count,taxi,&s->candidate,&distance)) return -1;
    TaxiPickupInput input={.candidate_index=s->candidate,.taxi_position=taxi,.gate_2b78=speed,
        .gate_1700=s->session.gate_1700,.random_first=random,.random_second=random};
    /* Pickup and trip fields project the same original020025D6 byte. */
    s->pickup.voice_25d6=s->session.trip.voice_25d6;
    TaxiPassengerState completion_prior={0};
    if(money_trace&&s->pickup.pending_1704&&s->pickup.selected_index>=0&&
        (unsigned)s->pickup.selected_index<s->count)
        completion_prior=s->customers[s->pickup.selected_index];
    TaxiPickupState trace_prior=s->pickup;
    uint32_t trace_rng=s->random_state;
    /* Source pickup draws only after these begin gates, before actors. */
    if(s->use_native_random&&!s->pickup.pending_1704&&s->pickup.destination_index==-1&&
        s->candidate>=0&&(unsigned)s->candidate<s->count&&
        (s->customers[s->candidate].bytes[0]&4)&&!speed&&!s->session.gate_1700){
        input.random_first=taxi_random_range_tagged(&s->random_state,0,65535,"pickup-first");
        if(s->customers[s->candidate].bytes[0]&8)
            input.random_second=taxi_random_range_tagged(&s->random_state,0,65535,"pickup-second");
    }
    if(taxi_passenger_pickup(&s->pickup,s->customers,s->destinations,s->count,&input,&effect)) return -1;
    if(pickup_trace&&(effect.began||effect.completed)){
        int32_t index=effect.customer_index;
        fprintf(pickup_trace,"%u %u %u %ld %ld %ld %ld %ld %ld %ld %u %u %u",
            (unsigned)effect.began,(unsigned)effect.completed,(unsigned)effect.random_calls,
            (long)input.candidate_index,(long)taxi.x,(long)taxi.z,(long)speed,
            (long)input.gate_1700,(long)input.random_first,(long)input.random_second,
            trace_rng,s->random_state,(unsigned)s->destinations[index]);
        trace_pickup_state(pickup_trace,&trace_prior);trace_pickup_state(pickup_trace,&s->pickup);
        for(unsigned j=0;j<sizeof s->customers[index].bytes;j++)
            fprintf(pickup_trace," %u",(unsigned)s->customers[index].bytes[j]);
        fputc('\n',pickup_trace);
    }
    if(effect.began)s->session.trip.voice_25d6=s->pickup.voice_25d6;
    if(reported)*reported=effect;
    /* Source b0da selection precedes cb84 fare initialization. */
    if(effect.completed && observer && observer(context,s,effect.destination_index))return -1;
    TaxiFareState money_prior=s->session.fare;
    TaxiTripStartState fare_prior=s->session.trip;
    uint32_t fare_rng_prior=s->random_state;
    /* Original completion dispatch precedes the actor update. */
    if(effect.completed&&s->use_native_random)
        random=taxi_random_range_tagged(&s->random_state,0,65535,"fare-pickup");
    if(effect.completed && taxi_fare_session_pickup(&s->session,
        s->customers+effect.customer_index,effect.customer_index,random)) return -1;
    if(effect.completed)s->pickup.voice_25d6=s->session.trip.voice_25d6;
    if(effect.completed&&money_trace){
        const uint8_t *customer=s->customers[effect.customer_index].bytes;
        fprintf(money_trace,"%ld %ld",(long)effect.customer_index,(long)word(customer+4));
        trace_money_state(money_trace,&money_prior);trace_money_state(money_trace,&s->session.fare);
        for(unsigned j=0;j<sizeof completion_prior.bytes;j++)fprintf(money_trace," %u",(unsigned)completion_prior.bytes[j]);
        for(unsigned j=0;j<sizeof completion_prior.bytes;j++)fprintf(money_trace," %u",(unsigned)customer[j]);
        fputc('\n',money_trace);
    }
    if(effect.completed&&fare_trace){
        const uint8_t *customer=s->customers[effect.customer_index].bytes;
        fprintf(fare_trace,"%ld %ld %ld %u %u %u %u %u %ld",
            (long)effect.customer_index,(long)word(customer+4),
            (long)s->session.clock.remaining_16a8,(unsigned)customer[2],
            (unsigned)s->session.settings.bytes[0],(unsigned)s->session.settings.bytes[3],
            fare_rng_prior,s->random_state,(long)random);
        trace_trip_state(fare_trace,&fare_prior);trace_trip_state(fare_trace,&s->session.trip);
        fputc('\n',fare_trace);
    }
    return 0;
}
static int customer_tick(TaxiPassengerScene *s,TaxiRoutePoint taxi,int32_t random,uint8_t freeze,int32_t delta) {
    if(taxi_passenger_marker_phase_step(&s->marker_phase,delta,freeze))return -1;
    s->render_entries_valid=1;
    for(unsigned i=0;i<s->count;i++) {
        const uint8_t *p=s->customers[i].bytes;
        uint32_t ux=(uint32_t)taxi.x-(uint32_t)word(p+16),uz=(uint32_t)taxi.z-(uint32_t)word(p+24);
        int32_t dx,dz;memcpy(&dx,&ux,4);memcpy(&dz,&uz,4);
        if(dx<0){ux=0u-ux;memcpy(&dx,&ux,4);}
        if(dz<0){uz=0u-uz;memcpy(&dz,&uz,4);}
        int entry_visible=dx<=12799&&dz<=12799&&p[1]<8&&(p[0]&3)==1;
        s->render_eligible[i]=(uint8_t)entry_visible;
        int selected=(int32_t)i==s->pickup.selected_index;
        int idle_status=s->use_native_random?
            taxi_passenger_idle_rng(s->customers+i,selected,&s->pickup,taxi,delta,freeze,&s->random_state):
            taxi_passenger_idle(s->customers+i,selected,&s->pickup,taxi,delta,freeze,random,120);
        if(idle_status||
            taxi_passenger_boarding(s->customers+i,selected,&s->pickup,taxi,delta,freeze)) return -1;
        /* Original22F34 culls before boarding movement, then2318E writes frame. */
        if(entry_visible&&taxi_passenger_frame_select(s->customers+i))return -1;
    }
    return 0;
}static int pickup_tick(TaxiPassengerScene *s,TaxiRoutePoint taxi,int32_t speed,int32_t random,TaxiPickupEffects *reported,TaxiPassengerSelectionObserver observer,void *context) {
    if(pickup_transition(s,taxi,speed,random,reported,observer,context))return -1;
    return customer_tick(s,taxi,random,0,1);
}

int taxi_passenger_scene_tick(TaxiPassengerScene *s,TaxiRoutePoint taxi,int32_t speed) {
    return pickup_tick(s,taxi,speed,0,NULL,NULL,NULL);
}
static int end_trip(TaxiPassengerScene *s,TaxiWorldAsset *world,const TaxiTrig *trig,
    const TaxiPassengerFrameInput *in,int success,TaxiPassengerSelectionObserver observer,void *context) {
    int32_t index=s->session.trip.active_customer;
    if(index<0||(unsigned)index>=s->count) return -1;
    if(success)s->session.gate_1700=120; /* b4ce..b4d0 precedes df8c. */
    s->pickup.destination_index=-1;
    if(observer && observer(context,s,-1))return -1;
    TaxiPassengerState *customer=s->customers+index;TaxiPoint exit;
    if(taxi_passenger_exit_position(world,trig,in->taxi,in->camera_word_10,
        word(customer->bytes+20),&exit)<0) return -1;
    int32_t random=s->use_native_random?taxi_random_range_tagged(&s->random_state,0,65535,"trip-end"):in->random_result;
    TaxiTripEndInput end={.settings_0=s->session.settings.bytes[0],.success=(uint8_t)success,
        .random_result=random,.exit_x=exit.x,.exit_y=exit.y,.exit_z=exit.z};
    TaxiTripEndEffects effect;
    return taxi_fare_session_end_display(&s->session,&s->pickup,customer,&end,&effect,in->cash_glyphs);
}
/* Split original BA04 clock/timeout services for early-frame scheduling.
   Effects accumulate so callers can retain ordered pickup/delivery events. */
int taxi_passenger_scene_clock_service_observed(TaxiPassengerScene *out,const TaxiPassengerAsset *asset,
    TaxiWorldAsset *world,const TaxiTrig *trig,const TaxiPassengerFrameInput *in,
    TaxiPassengerFrameEffects *effects,TaxiPassengerSelectionObserver observer,void *context){
    if(!out||!asset||!world||!trig||!in||!effects||(in->delta<1||in->delta>60)||out->count!=asset->count)return -1;
    if(in->rank_display&&!taxi_rank_display_owner_valid(in->rank_display))return -1;
    if(in->fade||in->start_active)return 0;
    TaxiPassengerScene s=*out;TaxiPassengerFrameEffects e=*effects;
    if(s.session.outer_state_1ea8||s.session.ended_1697){
        if(!in->freeze)e.expiry_fade=(uint8_t)taxi_fare_session_expiry_step(&s.session,in->delta);
        *out=s;*effects=e;return 0;
    }
    /* Preserve entry clock gates across all phases, even when main expires. */
    TaxiClockInput clock={.delta_3b00=in->delta,.gate_1700=s.session.gate_1700,
        .settings_0=s.session.settings.bytes[0],.freeze_5f20=in->freeze,.ended_1697=s.session.ended_1697};
    if(taxi_main_clock_step(&s.session.clock,&clock)) {s.session.ended_1697=1;e.main_expired=1;}
    if(!clock.freeze_5f20 && !clock.ended_1697)
        taxi_distance_display_step(&s.session.distance_display,&s.session.trip,
            in->delta,in->select_pressed);
    if(taxi_trip_clock_step(&s.session.trip,&clock)) {
        if(end_trip(&s,world,trig,in,0,observer,context)) return -1;
        e.timed_out=1;e.destination_events[e.destination_event_count++]=-1;
    }
    /* C316 result timer -> C540(-1) cash -> CA0C(0) rank, before C354
       time transfer. Preserve the ENTRY ended flag when time expires here. */
    TaxiSettlementDisplayInput display={in->fade,s.session.outer_state_1ea8,
        in->start_active,clock.ended_1697,in->freeze,s.session.settings.bytes[0],in->delta};
    if(taxi_settlement_display_step(&s.session.end,in->cash_glyphs,
        in->rank_display,s.session.rank_16d4,&display))return -1;
    e.palette_refresh=(uint8_t)taxi_clock_transfer(&s.session.clock,&s.session.trip,&s.session.end,&clock);
    taxi_fare_session_gate_step(&s.session,&clock);
    *out=s;*effects=e;return 0;
}

int taxi_passenger_scene_pickup_transition_observed(TaxiPassengerScene *out,const TaxiPassengerAsset *asset,
    TaxiWorldAsset *world,const TaxiTrig *trig,const TaxiPassengerFrameInput *in,
    TaxiPassengerFrameEffects *effects,TaxiPassengerSelectionObserver observer,void *context) {
    if(!out||!asset||!world||!trig||!in||!effects||(in->delta<1||in->delta>60)||out->count!=asset->count) return -1;
    TaxiPassengerScene s=*out;TaxiPassengerFrameEffects e=*effects;
    /* Original0800ba04 skips clocks for outerstate!=0. Existing ended
       entry takes0800bd5c transition branch and exits before0800c080. */
    if(s.session.outer_state_1ea8||s.session.ended_1697) {
        *out=s;*effects=e;return 0;
    }
    if(!in->freeze&&!s.session.ended_1697) {
        TaxiPickupEffects pickup_effect;
        if(pickup_transition(&s,(TaxiRoutePoint){in->taxi.x,in->taxi.z},in->speed,in->random_result,&pickup_effect,observer,context)) return -1;
        if(pickup_effect.completed)e.destination_events[e.destination_event_count++]=pickup_effect.destination_index;

    }
    *out=s;*effects=e;return 0;
}
int taxi_passenger_scene_customer_service(TaxiPassengerScene *out,const TaxiPassengerFrameInput *in) {
    if(!out||!in||(in->delta<1||in->delta>60)||out->count>128)return -1;
    if(out->session.outer_state_1ea8||out->session.ended_1697)return 0;
    TaxiPassengerScene s=*out;
    if(customer_tick(&s,(TaxiRoutePoint){in->taxi.x,in->taxi.z},in->random_result,in->freeze,in->delta))return -1;
    *out=s;return 0;
}
int taxi_passenger_scene_pickup_service_observed(TaxiPassengerScene *out,const TaxiPassengerAsset *asset,TaxiWorldAsset *world,const TaxiTrig *trig,const TaxiPassengerFrameInput *in,TaxiPassengerFrameEffects *effects,TaxiPassengerSelectionObserver observer,void *context) {
    if(!out||!effects)return -1;
    TaxiPassengerScene s=*out;TaxiPassengerFrameEffects e=*effects;
    if(taxi_passenger_scene_pickup_transition_observed(&s,asset,world,trig,in,&e,observer,context)||taxi_passenger_scene_customer_service(&s,in))return -1;
    *out=s;*effects=e;return 0;
}
int taxi_passenger_scene_delivery_service_observed(TaxiPassengerScene *out,const TaxiPassengerAsset *asset,
    TaxiWorldAsset *world,const TaxiTrig *trig,const TaxiPassengerFrameInput *in,
    TaxiPassengerFrameEffects *effects,TaxiPassengerSelectionObserver observer,void *context) {
    if(!out||!asset||!world||!trig||!in||!effects||(in->delta<1||in->delta>60)||out->count!=asset->count) return -1;
    if(in->rank_display&&!taxi_rank_display_owner_valid(in->rank_display))return -1;
    TaxiPassengerScene s=*out;TaxiPassengerFrameEffects e=*effects;
    /* Original0800ba04 skips clocks for outerstate!=0. Existing ended
       entry takes0800bd5c transition branch and exits before0800c080. */
    if(s.session.outer_state_1ea8||s.session.ended_1697) {
        *out=s;*effects=e;return 0;
    }
    if(!in->freeze&&!s.session.ended_1697) {
        int32_t destination=s.pickup.destination_index;
        if(destination>=0) {
            if((unsigned)destination>=asset->destination_count) return -1;
            TaxiDeliveryInput delivery={.destination_index=destination,.gate_1700=s.session.gate_1700,
                .pending_1704=s.pickup.pending_1704,.speed_2b78=in->speed,.material_e60=in->material,
                .active_1694=s.session.trip.active_1694,.gate_1708=in->delivery_gate,
                .destination_record=asset->destinations[destination]};
            TaxiDeliveryEffects accepted;
            if(taxi_passenger_delivery(&delivery,&accepted)) return -1;
            if(accepted.accepted) {
                if(end_trip(&s,world,trig,in,1,observer,context))return -1;
                /* B4F2 follows payout/numeric ranking; it starts AND steps once. */
                if(in->rank_display&&taxi_rank_display_owned_step(in->rank_display,
                    s.session.settings.bytes[0],s.session.rank_16d4,1,in->delta))return -1;
                e.delivered=1;e.destination_events[e.destination_event_count++]=-1;
            }
        }
    }
    *out=s;*effects=e;return 0;
}
int taxi_passenger_scene_late_service_observed(TaxiPassengerScene *out,const TaxiPassengerAsset *asset,TaxiWorldAsset *world,const TaxiTrig *trig,const TaxiPassengerFrameInput *in,TaxiPassengerFrameEffects *effects,TaxiPassengerSelectionObserver observer,void *context){
 if(!out||!effects)return -1;
 TaxiPassengerScene s=*out;TaxiPassengerFrameEffects e=*effects;
 if(taxi_passenger_scene_pickup_service_observed(&s,asset,world,trig,in,&e,observer,context)||taxi_passenger_scene_delivery_service_observed(&s,asset,world,trig,in,&e,observer,context))return -1;
 *out=s;*effects=e;return 0;
}

int taxi_passenger_scene_frame_observed(TaxiPassengerScene *out,const TaxiPassengerAsset *asset,
    TaxiWorldAsset *world,const TaxiTrig *trig,const TaxiPassengerFrameInput *in,
    TaxiPassengerFrameEffects *effects,TaxiPassengerSelectionObserver observer,void *context){
    if(!out||!asset||!world||!trig||!in||!effects||(in->delta<1||in->delta>60)||out->count!=asset->count)return -1;
    TaxiPassengerScene s=*out;TaxiPassengerFrameEffects e={0};
    if(taxi_passenger_scene_late_service_observed(&s,asset,world,trig,in,&e,observer,context))return -1;
    if(taxi_passenger_scene_clock_service_observed(&s,asset,world,trig,in,&e,observer,context))return -1;
    *out=s;*effects=e;return 0;
}

int taxi_passenger_scene_frame(TaxiPassengerScene *out,const TaxiPassengerAsset *asset,
    TaxiWorldAsset *world,const TaxiTrig *trig,const TaxiPassengerFrameInput *in,
    TaxiPassengerFrameEffects *effects) {
    return taxi_passenger_scene_frame_observed(out,asset,world,trig,in,effects,NULL,NULL);
}

static int challenge_passenger_frame_observed(uint8_t submode,TaxiPassengerScene *out,const TaxiPassengerAsset *asset,TaxiWorldAsset *world,const TaxiTrig *trig,const TaxiPassengerFrameInput *in,TaxiPassengerFrameEffects *effects,uint8_t *rank,TaxiPassengerSelectionObserver observer,void *context){
 if((submode!=4&&submode!=5)||!out||!asset||!world||!trig||!in||!effects||!rank||(in->delta<1||in->delta>60)||out->count!=asset->count||(out->session.settings.bytes[0]&224)!=128)return -1;
 TaxiPassengerScene s=*out;TaxiPassengerFrameEffects e={0};uint8_t request=0;
 if(s.session.outer_state_1ea8||s.session.ended_1697||in->freeze){*effects=e;*rank=0;return 0;}
 TaxiPickupEffects picked;
 if(pickup_tick(&s,(TaxiRoutePoint){in->taxi.x,in->taxi.z},in->speed,in->random_result,&picked,observer,context))return -1;
 if(picked.completed)e.destination_events[e.destination_event_count++]=picked.destination_index;
 int32_t destination=s.pickup.destination_index;
 if(destination>=0){
  if((unsigned)destination>=asset->destination_count)return -1;
  TaxiChallengeDeliveryInput gate={.submode1537=submode,.delivery={.destination_index=destination,.destination_record=asset->destinations[destination],.gate_1700=s.session.gate_1700,.pending_1704=s.pickup.pending_1704,.speed_2b78=in->speed,.material_e60=in->material,.active_1694=s.session.trip.active_1694}};
  TaxiDeliveryEffects accepted;if(taxi_challenge_delivery_accept(&gate,&accepted))return -1;
  if(accepted.accepted){
   int32_t index=s.session.trip.active_customer;if(index<0||(unsigned)index>=s.count)return -1;
   s.session.gate_1700=120;s.pickup.destination_index=-1;
   if(observer&&observer(context,&s,-1))return -1;
   TaxiPassengerState *actor=s.customers+index;TaxiPoint exit;
   if(taxi_passenger_exit_position(world,trig,in->taxi,in->camera_word_10,word(actor->bytes+20),&exit)<0)return -1;
   TaxiTripEndInput end={.settings_0=s.session.settings.bytes[0],.success=1,.exit_x=exit.x,.exit_y=exit.y,.exit_z=exit.z};TaxiTripEndEffects ended;
   if(taxi_fare_session_end_turn_display(&s.session,&s.pickup,actor,&end,&ended,&request,in->cash_glyphs))return -1;
   e.delivered=1;e.destination_events[e.destination_event_count++]=-1;
  }
 }
 *out=s;*effects=e;*rank=request;return 0;
}

int taxi_passenger_scene_turn_frame_observed(TaxiPassengerScene *out,const TaxiPassengerAsset *asset,TaxiWorldAsset *world,const TaxiTrig *trig,const TaxiPassengerFrameInput *in,TaxiPassengerFrameEffects *effects,uint8_t *rank,TaxiPassengerSelectionObserver observer,void *context){
 return challenge_passenger_frame_observed(4,out,asset,world,trig,in,effects,rank,observer,context);
}
int taxi_passenger_scene_bound_frame_observed(TaxiPassengerScene *out,const TaxiPassengerAsset *asset,TaxiWorldAsset *world,const TaxiTrig *trig,const TaxiPassengerFrameInput *in,TaxiPassengerFrameEffects *effects,uint8_t *rank,TaxiPassengerSelectionObserver observer,void *context){
 /* Shared0790E delivery branch accepts mode5; both4/5 have identical nonzero-clock rank gates. */
 return challenge_passenger_frame_observed(5,out,asset,world,trig,in,effects,rank,observer,context);
}
int taxi_passenger_scene_turn_frame(TaxiPassengerScene *out,const TaxiPassengerAsset *asset,TaxiWorldAsset *world,const TaxiTrig *trig,const TaxiPassengerFrameInput *in,TaxiPassengerFrameEffects *effects,uint8_t *rank){
 return taxi_passenger_scene_turn_frame_observed(out,asset,world,trig,in,effects,rank,NULL,NULL);
}
