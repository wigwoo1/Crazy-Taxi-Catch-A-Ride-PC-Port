#ifdef NDEBUG
#undef NDEBUG
#endif
#include "game_audio_controller.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static TaxiGameAudioControllerInput input(void){TaxiGameAudioControllerInput i={0};i.max_speed=1000;i.delta=1;i.vehicle564=1;i.gate00e4=1;i.current_class=0;i.alternate_class=1;return i;}
static unsigned starts(const TaxiGameAudioEngineRequest *q,unsigned n,unsigned index){unsigned x=0;for(unsigned j=0;j<n;j++)x+=q[j].op==1&&q[j].handle==index;return x;}
int main(void){TaxiGameAudioController s={0};s.factor=256;s.driver_selection=0;s.context_selection=0;s.amplitude=0;s.handles[9]=123;
 TaxiGameAudioControllerInput i=input();i.starts[0]=10;i.starts[8]=80;i.starts[7]=70;TaxiGameAudioEngineRequest q[32];unsigned n;
 assert(!taxi_game_audio_controller_step(&s,&i,q,&n));assert(s.handles[8]==80&&!s.handles[7]);assert(starts(q,n,131)==1&&starts(q,n,83)==0);assert(q[1].op==2&&q[1].value==0);assert(s.amplitude==128&&s.handles[9]==123);
 /* Driver finishes first: contextual voice may start in the same frame. */
 assert(!taxi_game_audio_controller_step(&s,&i,q,&n));assert(!s.handles[8]&&s.handles[7]==70);assert(starts(q,n,83)==1);
 /* Context ends after driver block: followup starts next frame, not now. */
 i.random=1;assert(!taxi_game_audio_controller_step(&s,&i,q,&n));assert(!s.handles[7]&&!s.handles[8]&&s.driver_selection==8);assert(starts(q,n,139)==0);
 assert(!taxi_game_audio_controller_step(&s,&i,q,&n));assert(s.handles[8]==80&&starts(q,n,139)==1);
 /* Slot5 pending start is checked for completion later in that same frame. */
 s.pending3=1;i.starts[5]=50;i.busy[5]=0;assert(!taxi_game_audio_controller_step(&s,&i,q,&n));assert(!s.handles[5]&&!s.pending3&&starts(q,n,44)==1);
 TaxiGameAudioController before=s;memset(q,0x55,sizeof q);TaxiGameAudioEngineRequest old[32];memcpy(old,q,sizeof q);n=99;i.max_speed=0;
 assert(taxi_game_audio_controller_step(&s,&i,q,&n)==-1);assert(!memcmp(&s,&before,sizeof s)&&!memcmp(q,old,sizeof q)&&n==99);
 puts("PASS original-order voice handoffs, delayed followup, slot5 completion, amplitude timing, slot9 retention and transactional rejection");return 0;}
