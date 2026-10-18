#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/world_update.h"
#include "../src/spawn_placement.h"
static TaxiTrig trig;
static TaxiNormalAngles angles;
typedef struct {int result;TaxiPoint seen;TaxiGroundHit hit;} Fixture;
static int obstacle(void *ctx,TaxiPoint p,TaxiObstacleHit *h) {(void)ctx;(void)p;(void)h;return 0;}
static int ground(void *ctx,TaxiPoint p,TaxiGroundHit *h) {Fixture *f=ctx;f->seen=p;*h=f->hit;return f->result;}
static TaxiWorldState initial(void) {
    TaxiWorldState v={.height={.position={0,100,0},.scale_e84=4096},.ground_36c=77,
        .red_374=128,.green_375=128,.blue_376=128,.contact_378=123,.speed_544=4096,
        .state_40=4096,.state_44=4096,.state_2c=42,.displacement_x=12};
    v.height.body.denominator_754=57344;v.height.body.denominator_758=28672;return v;
}
int main(void) {
    for(unsigned i=0;i<4096;i++) trig.cosine[i]=256;
    trig.cosine[1024]=0;trig.sine[1024]=255;
    TaxiPoint p=taxi_rotate_y((TaxiPoint){256,10,512},1024,&trig);
    assert(p.x==-510&&p.y==10&&p.z==255);
    trig.cosine[0]=255;p=taxi_rotate_y((TaxiPoint){256,10,512},0,&trig);
    assert(p.x==255&&p.y==10&&p.z==510);trig.cosine[0]=256;
    TaxiWorldState v=initial();TaxiHeightGlobals g={0};TaxiHeightInput in={.delta=1};TaxiWorldEffects e;
    Fixture f={.result=1,.hit={.height=100,.normal={0,4096,0},.polygon=17,.material=123,.contact_valid=1,.contact_value=456}};
    v.impulse_34=60000;v.impulse_74c=40960;v.impulse_750=-40960;
    assert(!taxi_world_update(&v,&g,&in,&trig,&angles,obstacle,ground,&f,&e));
    assert(v.height.position.x==22&&v.height.position.z==-10&&v.height.position.y==100);
    assert(f.seen.y==77&&f.seen.x==22&&f.seen.z==-10&&e.ground_hit);
    assert(v.heading_1c==1000&&v.heading_20==1000&&v.impulse_34==52500&&v.impulse_74c==35840);
    assert(v.speed_544==3993&&v.state_40==3993&&v.state_44==3993&&!v.state_2c);
    assert(v.height.flags_28==16&&v.material_e60==123&&v.ground_36c==100&&v.height_3c==100);
    assert(v.red_374==112&&v.green_375==112&&v.blue_376==112&&v.previous_37c==123&&v.contact_378==456);
    assert(v.snapshot_714.material==123&&v.snapshot_714.contact_value==456);
    v=initial();v.red_374=0;v.height.flags_28=16;f.hit.contact_valid=0;
    assert(!taxi_world_update(&v,&g,&in,&trig,&angles,obstacle,ground,&f,&e));
    assert(v.red_374==1&&v.speed_544==4096&&v.state_40==4014&&v.contact_378==0);
    v=initial();v.height.state_698=900;v.height.flags_28=8;v.snapshot_714.height=456;f.result=0;
    assert(!taxi_world_update(&v,&g,&in,&trig,&angles,obstacle,ground,&f,&e));
    assert(!e.ground_hit&&v.height.position.y==100&&v.height.flags_28==8&&v.height.state_698==0);
    assert(v.material_e60==65535&&v.ground_36c==77&&v.snapshot_714.height==456&&v.red_374==128);
    f.result=1;f.hit.normal=(TaxiPoint){0};v=initial();
    assert(!taxi_world_update(&v,&g,&in,&trig,&angles,obstacle,ground,&f,&e)&&!e.ground_hit&&v.material_e60==65535);
    f.hit.normal=(TaxiPoint){0,4096,0};f.result=-1;v=initial();TaxiWorldState old=v;TaxiHeightGlobals oldg=g;TaxiWorldEffects olde=e;
    assert(taxi_world_update(&v,&g,&in,&trig,&angles,obstacle,ground,&f,&e)==-1);
    assert(!memcmp(&v,&old,sizeof v)&&!memcmp(&g,&oldg,sizeof g)&&!memcmp(&e,&olde,sizeof e));
    f.result=1;v.height.body.denominator_758=0;old=v;
    assert(taxi_world_update(&v,&g,&in,&trig,&angles,obstacle,ground,&f,&e)==-1&&!memcmp(&v,&old,sizeof v));
    uint8_t bytes[16]={0,0,0,0,0xff,0xff,0xff,0xff,2,0,0,0,0xff,0xff,0,0};
    TaxiSpawnMarker marker;assert(!taxi_spawn_marker_decode(&marker,bytes,sizeof bytes));
    assert(marker.query.x==-1&&marker.query.y==INT32_MIN&&marker.query.z==2&&marker.heading==4095);
    TaxiSpawnMarker saved=marker;
    assert(taxi_spawn_marker_decode(&marker,bytes,15)==-1&&!memcmp(&marker,&saved,sizeof marker));
    v=initial();int32_t back=0;
    assert(taxi_spawn_place(&v,&back,&marker,0,ground,&f)==1);
    assert(f.seen.y==INT32_MIN&&v.height.position.x==-1&&v.height.position.y==164&&v.height.position.z==2);
    assert(v.heading_1c==4095*4096&&v.heading_20==v.heading_1c&&back==2047*4096);
    assert(taxi_spawn_place(&v,&back,&marker,0x80,ground,&f)==1&&back==4095*4096);
    f.result=0;v=initial();assert(!taxi_spawn_place(&v,&back,&marker,0,ground,&f));
    assert(v.height.position.x==0&&v.height.position.y==100&&v.ground_36c==77&&v.heading_1c==4095*4096);
    f.result=-1;old=v;int32_t oldback=back;
    assert(taxi_spawn_place(&v,&back,&marker,0,ground,&f)==-1&&!memcmp(&old,&v,sizeof v)&&back==oldback);
    puts("PASS: native world ordering, caches, pose, original-marker placement, no-hit and rollback.");return 0;
}

