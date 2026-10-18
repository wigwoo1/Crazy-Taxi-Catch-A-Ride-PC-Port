#include "reward_session.h"
#include <string.h>
int taxi_reward_session_reset(TaxiFareSession *s){
 if(!s)return -1;
 s->reward_clock.clock_9aa5=0;s->reward_gate_9aa4=0;
 s->combo_whole_9aa8=0;s->combo_fraction_9aac=0;s->end.flag_9aa6=0;
 memset(s->tips,0,sizeof s->tips);return 0;
}
static FILE *trace_sink,*update_sink;
void taxi_reward_session_update_trace(FILE *sink){update_sink=sink;}static TaxiRewardStats stats;
void taxi_reward_session_trace(FILE *sink){trace_sink=sink;memset(&stats,0,sizeof stats);}
TaxiRewardStats taxi_reward_session_stats(void){return stats;}
static void trace_state(const TaxiRewardState *r){
 fprintf(trace_sink," %u %u %u %ld %ld %u",r->gate_9aa4,r->combo_9aa6,r->voice_25d6,(long)r->whole_9aa8,(long)r->fraction_9aac,r->random_state);
 for(unsigned i=0;i<10;i++)for(unsigned j=0;j<56;j++)fprintf(trace_sink," %u",r->tips[i][j]);
}
static TaxiRewardState project(const TaxiFareSession *s,uint32_t rng){
 TaxiRewardState r={.gate_9aa4=s->reward_gate_9aa4,.combo_9aa6=s->end.flag_9aa6,.voice_25d6=s->trip.voice_25d6,.whole_9aa8=s->combo_whole_9aa8,.fraction_9aac=s->combo_fraction_9aac,.random_state=rng};
 memcpy(r.tips,s->tips,sizeof r.tips);return r;
}
static void commit(TaxiFareSession *s,const TaxiRewardState *r){
 s->reward_gate_9aa4=r->gate_9aa4;s->end.flag_9aa6=r->combo_9aa6;s->trip.voice_25d6=r->voice_25d6;
 s->combo_whole_9aa8=r->whole_9aa8;s->combo_fraction_9aac=r->fraction_9aac;memcpy(s->tips,r->tips,sizeof s->tips);
}
int taxi_reward_session_request(TaxiFareSession *s,uint32_t *rng,const TaxiRewardInput *in,TaxiRewardEffects *e){
 if(!s||!rng||!in||!e)return -1;TaxiRewardState r=project(s,*rng);
 if(trace_sink){fprintf(trace_sink,"%u %u %u %u %ld %ld",in->kind,in->active_1694,in->settings_0,in->sound_enabled,(long)in->whole,(long)in->fraction);trace_state(&r);}
 if(taxi_reward(&r,in,e))return -1;stats.requests++;if(e->slot>=0)stats.created++;
 if(trace_sink){trace_state(&r);fprintf(trace_sink," %d %d %u\n",e->slot,e->sound_table_index,e->random_calls);}
 commit(s,&r);*rng=r.random_state;return 0;
}
static void trace_update_state(const TaxiFareSession *s,const TaxiCashGlyphs *g){
 fprintf(update_sink," %u %u %u %ld %ld",s->reward_gate_9aa4,s->reward_clock.clock_9aa5,s->reward_clock.active_count_9aa7,(long)s->fare.pending_whole_16a0,(long)s->fare.pending_fraction_16a4);
 for(unsigned i=0;i<9;i++)fprintf(update_sink," %u",g->pending_target[i]);
 for(unsigned i=0;i<10;i++)for(unsigned j=0;j<56;j++)fprintf(update_sink," %u",s->tips[i][j]);
}
int taxi_reward_session_update(TaxiFareSession *s,TaxiCashGlyphs *g,int32_t delta,uint8_t sound,TaxiRewardUpdateEffects *e){
 if(!s||!g||!e)return -1;TaxiRewardState r=project(s,0);
 int capture=0;if(update_sink)for(unsigned i=0;i<10;i++)if(s->tips[i][2])capture=1;
 if(capture){fprintf(update_sink,"%ld %u",(long)delta,sound);trace_update_state(s,g);}
 if(taxi_reward_update(&r,&s->reward_clock,&s->fare,g,delta,sound,e))return -1;
 stats.updates++;stats.expired+=e->expired;if(s->reward_clock.active_count_9aa7)stats.active_updates++;
 commit(s,&r);if(capture){trace_update_state(s,g);fprintf(update_sink," %u %u\n",e->expired,e->sound_requests);}return 0;
}
