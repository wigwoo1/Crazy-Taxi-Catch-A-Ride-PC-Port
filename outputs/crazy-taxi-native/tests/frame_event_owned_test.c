#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/frame_event.h"
#include "../src/save_block.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
typedef struct {unsigned n,fail;uint32_t targets[3],args[3];uint8_t expected,final,mutate,keep;} Probe;
static int call(void *opaque,TaxiFrameEvent *s,uint32_t target,uint32_t argument){
 Probe *p=opaque;assert(p->n<3);p->targets[p->n]=target;p->args[p->n]=argument;p->n++;
 if(target==0x08026888){if(p->mutate){s->preserve_1ea7=p->keep;s->settings_1eb0=0xf1;}}
 else if(target==0x0803244c){assert(s->settings_1eb0==p->expected);if(p->mutate)s->settings_1eb0=p->final;}
 else {assert(target==0x080161f4&&s->settings_1eb0==p->final);}
 return p->fail==p->n?-1:0;
}
int main(void){
 const uint32_t counters[]={0,UINT32_MAX,0x7fffffff,0x80000000};
 const uint32_t callbacks[]={0,0x080161d9,0x080161dd,0x08017001,UINT32_MAX};
 const uint8_t keep[]={0,1,255};unsigned cases=0;
 for(unsigned value=0;value<256;value++)for(unsigned k=0;k<3;k++)for(unsigned c=0;c<4;c++)for(unsigned r=0;r<5;r++){
  TaxiFrameEvent first={.counter_3520=counters[c],.scanline_4308=0xa5a5,.callback_352c=callbacks[r],.preserve_1ea7=keep[k],.settings_1eb0=(uint8_t)value,.acknowledge=0xa5a5},owned=first;
  uint8_t expected=keep[k]?(uint8_t)value:(uint8_t)((value&0xe7)|8);
  Probe a={.expected=expected,.final=expected},b=a;
  assert(!taxi_frame_event(&first,160,call,&a));
  assert(!taxi_frame_event_after_counter(&owned,(uint32_t)(counters[c]+1),call,&b));
  assert(owned.counter_3520==(uint32_t)(counters[c]+1)&&owned.scanline_4308==0xa5a5&&owned.settings_1eb0==expected&&owned.acknowledge==1);
  unsigned count=callbacks[r]?3:2;assert(a.n==count&&b.n==count);
  assert(a.targets[0]==0x08026888&&a.targets[count-1]==0x080161f4);
  if(callbacks[r])assert(a.targets[1]==0x0803244c&&a.args[1]==callbacks[r]);
  assert(!memcmp(&a,&b,sizeof a));first.scanline_4308=owned.scanline_4308;assert(!memcmp(&first,&owned,sizeof first));cases++;
 }
 TaxiFrameEvent state={.counter_3520=42,.settings_1eb0=0xf0,.acknowledge=0xa5a5},before=state;
 Probe none={.expected=0xe8,.final=0xe8};
 assert(taxi_frame_event_after_counter(&state,42,call,&none)==-1&&!none.n&&!memcmp(&state,&before,sizeof state));
 assert(taxi_frame_event_after_counter(&state,44,call,&none)==-1&&!none.n&&!memcmp(&state,&before,sizeof state));
 assert(taxi_frame_event_after_counter(NULL,43,call,&none)==-1);
 assert(taxi_frame_event_after_counter(&state,43,NULL,&none)==-1&&!memcmp(&state,&before,sizeof state));
 for(unsigned r=0;r<5;r++){state=before;assert(!taxi_frame_event_register(&state,callbacks[r]));assert(state.callback_352c==(callbacks[r]?callbacks[r]:0x080161dd));state.callback_352c=before.callback_352c;assert(!memcmp(&state,&before,sizeof state));}
 assert(taxi_frame_event_register(NULL,0)==-1);
 assert(taxi_frame_event_callback_is_noop(0x080161d9)&&taxi_frame_event_callback_is_noop(0x080161dd));
 assert(!taxi_frame_event_callback_is_noop(0)&&!taxi_frame_event_callback_is_noop(0x080161d8)&&!taxi_frame_event_callback_is_noop(0x08017001));
 for(unsigned k=0;k<2;k++){state=before;state.callback_352c=0x080161dd;Probe p={.expected=k?0xf1:0xe9,.final=0xa4,.mutate=1,.keep=(uint8_t)k};assert(!taxi_frame_event_after_counter(&state,43,call,&p)&&state.settings_1eb0==0xa4&&p.n==3);}
 for(unsigned fail=1;fail<=3;fail++){state=before;state.callback_352c=0x080161dd;Probe p={.expected=0xe8,.final=0xe8,.fail=fail};assert(taxi_frame_event_after_counter(&state,43,call,&p)==-1&&state.counter_3520==43&&state.acknowledge==0xa5a5&&p.n==fail);}
 uint8_t block[512],rows[800],table[80]={0};taxi_save_block_defaults(block,rows,table);
 for(unsigned value=0;value<256;value++){
  block[0]=(uint8_t)value;state=(TaxiFrameEvent){.settings_1eb0=block[0]};
  uint8_t expected=(uint8_t)((value&0xe7)|8);Probe p={.expected=expected,.final=expected};
  assert(!taxi_frame_event_after_counter(&state,1,call,&p));block[0]=state.settings_1eb0;
  taxi_save_block_prepare(block);assert(!(block[0]&24)&&block[0]==(value&0xe7)&&taxi_save_block_checksum_valid(block));
 }
 printf("PASS %u source software-event vectors, native registration/default callbacks, single counter/wrap, post-audio settings and callback mutation, atomic wrong-counter rejection, failure acknowledgments, transient-save normalization\n",cases);
 return 0;
}
