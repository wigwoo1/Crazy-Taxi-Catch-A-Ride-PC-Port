#include "screen_sequence.h"
#include <string.h>
int taxi_screen_sequence_prepare(TaxiScreenSequence *s,TaxiScreenDescriptor *d,TaxiScreenSequenceCall call,void *context){
 if(!s||!d||!call)return -1;
 memcpy(&s->flow.remaining_359c,&d->timer,4);s->flow.elapsed=0;
 s->audio_755c=0;s->state_3568=0;s->count_356c=0;s->limit_356e=16;
 s->state_356a=0;s->animation_3574=255;s->callback_3570=0;
 return call(context,s,d,0x0800e5a8,1,32,d->fade_in_bg,d->fade_in_obj)?-1:0;
}
int taxi_screen_sequence_advance(TaxiScreenSequence *s,TaxiScreenDescriptor *next,TaxiScreenSequenceCall call,void *context){
 if(!s||!next||!call)return -1;
 s->descriptor_slot++;s->index=(uint16_t)(s->index+1u);
 if(call(context,s,next,0x0802763c,0,0,0,0))return -1;
 return next->image!=0||next->type!=-1;
}
