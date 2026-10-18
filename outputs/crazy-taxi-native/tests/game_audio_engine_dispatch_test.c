#ifdef NDEBUG
#undef NDEBUG
#endif
#include "game_audio_engine.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
typedef struct {TaxiGameAudioEngineRequest q[6];unsigned n;uint32_t starts[2],available;} Context;
static uint32_t fixed(void *v,TaxiGameAudioEngineRequest q){Context *c=v;assert(c->n<6);c->q[c->n++]=q;return q.op==1?c->starts[q.handle-9]:0;}
static uint32_t reuse(void *v,TaxiGameAudioEngineRequest q){Context *c=v;c->q[c->n++]=q;if(q.op==4){c->available=q.handle;return 0;}if(q.op==1){uint32_t h=c->available;c->available=0;return h;}return 0;}
int main(void){unsigned cases=0;const int32_t levels[]={0,1,48,-1},amps[]={0,16384,2147483647};
 for(unsigned a=0;a<2;a++)for(unsigned b=0;b<2;b++)for(unsigned lo=0;lo<4;lo++)for(unsigned hi=0;hi<4;hi++)for(unsigned amp=0;amp<3;amp++)for(unsigned fail=0;fail<2;fail++){
  TaxiGameAudioEngine s={{a?11:0,b?12:0}},t=s;Context c={0};c.starts[0]=fail?0:21;c.starts[1]=fail?0:22;TaxiGameAudioEngineRequest q[6];unsigned n=taxi_game_audio_engine_step(&s,levels[lo],levels[hi],amps[amp],10512,c.starts,q);
  assert(taxi_game_audio_engine_dispatch(&t,levels[lo],levels[hi],amps[amp],10512,fixed,&c)==n);assert(!memcmp(&s,&t,sizeof s)&&n==c.n);for(unsigned j=0;j<n;j++)assert(q[j].op==c.q[j].op&&q[j].handle==c.q[j].handle&&q[j].value==c.q[j].value);cases++;
 }
 TaxiGameAudioEngine s={{17,0}};Context c={0};assert(taxi_game_audio_engine_dispatch(&s,0,48,16384,10512,reuse,&c)==4);assert(s.handle[0]==0&&s.handle[1]==17);assert(c.q[0].op==4&&c.q[1].op==1&&c.q[2].handle==17&&c.q[3].handle==17);
 printf("PASS %u exact engine request/state cases and sequential stop-before-start channel reuse\n",cases);return 0;}
