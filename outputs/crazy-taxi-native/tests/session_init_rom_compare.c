#include "session_init.h"
#include <assert.h>
#include <stdio.h>
static void output(const uint8_t *p,unsigned n){for(unsigned i=0;i<n;i++)printf("%02x",p[i]);}
int main(void){unsigned mode,seconds,alternate,challenge,width,seed;while(scanf("%u %u %u %u %u %u",&mode,&seconds,&alternate,&challenge,&width,&seed)==6){
 TaxiSessionInit s;uint8_t *owners[]={s.shared_1534,s.session_165e,s.sound_2618,s.effect_5f20,s.projection_5310,s.scroll_7530,s.settings,s.snapshot};unsigned sizes[]={8,174,24,12,6,16,512,512};
 for(unsigned j=0;j<8;j++)for(unsigned i=0;i<sizes[j];i++)owners[j][i]=(uint8_t)(seed+i*37);
 s.outer_1ea8=(uint8_t)seed;s.reset_430c=(uint16_t)seed;s.counter_67d0=seed;
 for(unsigned i=0;i<12;i++)s.effect_slots[i]=(int)seed;
 s.settings[0]=(uint8_t)mode;s.settings[5]=(uint8_t)seconds;s.session_165e[170]=(uint8_t)alternate;s.shared_1534[3]=(uint8_t)challenge;
 assert(!taxi_session_init(&s,(uint16_t)width));for(unsigned j=0;j<8;j++)output(owners[j],sizes[j]);
 printf(" %u %u %u",s.outer_1ea8,s.reset_430c,s.counter_67d0);for(unsigned i=0;i<12;i++)printf(" %d",s.effect_slots[i]);puts("");
}return 0;}
