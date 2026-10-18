#include "passenger_init_world.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
typedef struct {TaxiMaterialBlink *s;TaxiPassengerInitPending *p;unsigned permit,panels,fail;} Services;
static void put(uint8_t *p,int32_t v){uint32_t u=(uint32_t)v;for(unsigned j=0;j<4;j++)p[j]=(uint8_t)(u>>(j*8));}
static int32_t word(const uint8_t *p){uint32_t u=p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;int32_t v;memcpy(&v,&u,4);return v;}
static int loading(void *ctx,unsigned pass,size_t index){Services *s=ctx;assert(pass==s->p->pass&&index==0);if(!s->permit)return 1;s->permit=0;return 0;}
static int spawn(void *ctx,size_t i,const uint8_t source[32]){(void)ctx;(void)i;(void)source;assert(0);return -1;}
static int panel(void *ctx,const uint8_t record[48],const uint8_t cache[512]){
 Services *s=ctx;assert(s->s->destination==0&&s->s->has_record&&record[40]==7&&s->s->indices[0]==7);
 if(s->fail)return -1;
 for(unsigned i=0;i<512;i++)assert(cache[i]==(s->panels?0:0xa5));s->panels++;return 0;
}
int main(int argc,char **argv){
 assert(argc==2);TaxiWorldAsset world={0};assert(!taxi_world_asset_load(&world,argv[1]));
 uint8_t source[1][32]={{0}},dest[1][48]={{0}};source[0][3]=3;put(source[0]+4,3350);put(source[0]+12,128);put(dest[0]+24,100);put(dest[0]+32,64);memset(dest[0]+40,255,8);dest[0][40]=7;dest[0][41]=0;
 TaxiRoutePoint points[2]={{3350,128},{100,64}};TaxiPassengerRouteAsset route={0,2,points};TaxiPassengerAsset asset={.count=1,.source=source,.route_count=1,.routes=&route,.destination_count=1,.destinations=dest};
 TaxiPassengerState actor;memset(&actor,0xa5,sizeof actor);put(actor.bytes+20,-128);TaxiPassengerState prior=actor;
 TaxiMaterialBlink selection={.destination=-1,.clock=99};memset(selection.cache,0xa5,512);uint8_t settings=0,override=0,animation=0;
 TaxiPassengerInitPending p={.states=&actor,.asset=&asset,.settings_1eb0=&settings,.override_1708=&override,.animation_1684=&animation};Services services={&selection,&p,0,0,0};
 TaxiPassengerInitWorld binding={.asset=&asset,.world=&world,.selection=&selection,.settings_1eb0=&settings,.contact={.material=7,.height=123,.contact_value=0xabcdef},.panel=panel,.panel_context=&services,.service_context=&services,.loading=loading,.spawn=spawn};
 TaxiPassengerInitBackend untouched={0},bad=untouched;assert(taxi_passenger_init_world_bind(&bad,NULL)==-1&&!memcmp(&bad,&untouched,sizeof bad));
 TaxiPassengerInitWorld invalid=binding;invalid.panel=NULL;assert(taxi_passenger_init_world_bind(&bad,&invalid)==-1&&!memcmp(&bad,&untouched,sizeof bad));
 assert(!taxi_passenger_init_world_bind(&p.backend,&binding));TaxiWorldContact before=binding.contact;
 assert(!taxi_passenger_init_pending_begin(&p,777));
 for(unsigned i=0;i<4;i++)assert(!taxi_passenger_init_pending_step(&p)&&!memcmp(&actor,&prior,sizeof actor)&&animation==1&&services.panels==0);
 services.permit=1;assert(!taxi_passenger_init_pending_step(&p)&&p.pass==1&&actor.bytes[0]==1&&word(actor.bytes+20)==-128&&services.panels==1&&animation==2);
 prior=actor;for(unsigned i=0;i<4;i++)assert(!taxi_passenger_init_pending_step(&p)&&!memcmp(&actor,&prior,sizeof actor)&&services.panels==1);
 services.permit=1;assert(taxi_passenger_init_pending_step(&p)==1&&actor.bytes[0]==1&&actor.bytes[1]==3&&word(actor.bytes+20)==-128&&services.panels==2&&selection.destination==-1&&selection.clock==99);
 assert(!memcmp(&binding.contact,&before,sizeof before)&&p.endpoint.y==777&&word(actor.bytes+4)>0);
 /* An absent-cell endpoint is eligible through retained material7 even though
    original2ABA0 returns false. A valid-cell miss writes65535 instead. */
 TaxiPoint point={1024,0,64};uint32_t material=0;assert(!p.backend.endpoint(p.backend.context,&point,&material)&&material==65535&&binding.contact.height==123&&binding.contact.contact_value==0xabcdef);
 point=(TaxiPoint){100,0,64};binding.contact.material=9;assert(!p.backend.endpoint(p.backend.context,&point,&material)&&material==9);
 put(actor.bytes+16,100);put(actor.bytes+24,64);int32_t height=99;assert(!p.backend.height(p.backend.context,&actor,&height)&&height==INT32_MIN);
 memset(selection.cache,0x5a,512);services.fail=1;assert(p.backend.select(p.backend.context,0)==-1&&selection.destination==0&&selection.has_record);
 for(unsigned i=0;i<512;i++)assert(selection.cache[i]==0x5a);
 assert(!p.backend.select(p.backend.context,-1)&&selection.destination==-1&&!selection.has_record);for(unsigned i=0;i<512;i++)assert(!selection.cache[i]);
 int accepted=1;assert(!p.backend.accept(p.backend.context,7,&accepted)&&!accepted);
 taxi_world_asset_free(&world);puts("PASS native passenger/world retained-miss eligibility, runtimeY, repeated pending waits, destination panel/cache failure retention and invalid binding checks");return 0;
}
