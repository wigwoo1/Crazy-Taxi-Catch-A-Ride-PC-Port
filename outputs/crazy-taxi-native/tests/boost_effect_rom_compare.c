#include "../src/effect_emit.h"
#include <stdio.h>
#include <string.h>
int main(void){unsigned flags,mat,alt,phase,voice,ring;int scale,timer,delta,speed,state,clock;while(scanf("%u%u%u%u%u%u%d%d%d%d%d%d",&flags,&mat,&alt,&phase,&voice,&ring,&scale,&timer,&delta,&speed,&state,&clock)==12){TaxiEffectPool p;memset(&p,0x5a,sizeof p);TaxiBoostEffect s={.actor_flags=flags,.material_flags=mat,.alternate=alt,.scale=scale,.timer=timer,.delta=delta,.clock=clock,.attachment_tags={2,3},.velocity_basis={{-256,256,-3072},{256,256,-3072}},.action2=phase};(void)speed;(void)state;(void)voice;int rc=taxi_effect_emit_boost(&p,&ring,&s);printf("%d %u %d",rc,ring,s.timer);for(unsigned i=0;i<sizeof p.records;i++)printf(" %u",((unsigned char*)p.records)[i]);puts("");}return 0;}
