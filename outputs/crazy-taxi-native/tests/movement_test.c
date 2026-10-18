#include "../src/movement.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <limits.h>
#include <stdio.h>
typedef struct {int kind;int32_t height;TaxiPoint last;} Fixture;
static int query(void *context,TaxiPoint p,TaxiObstacleHit *hit) {
    Fixture *f=context;f->last=p;hit->height=f->height;
    if(f->kind==2) {
        hit->height=INT_MAX;hit->edge_count=4;
        hit->vertices[0]=(TaxiPoint){0,0,0};hit->vertices[1]=(TaxiPoint){0,0,100};
        hit->vertices[2]=(TaxiPoint){100,100,100};hit->vertices[3]=(TaxiPoint){100,100,0};
    }
    return f->kind;
}
int main(void) {
    TaxiMovementResult r;Fixture f={0,INT_MIN,{0}};
    assert(taxi_move_position((TaxiPoint){0,10,0},(TaxiPoint){1000,0,0},0,query,&f,&r)==0);
    assert(r.position.x==1000 && r.position.y==10 && r.queries==4 && !r.collided);
    assert(taxi_move_position((TaxiPoint){0,10,0},(TaxiPoint){1001,0,0},0,query,&f,&r)==0);
    assert(r.position.x==1000); /* Original discards division remainder. */
    assert(taxi_move_position((TaxiPoint){0,10,0},(TaxiPoint){-1001,0,0},0,query,&f,&r)==0);
    assert(r.position.x==-1000);
    assert(taxi_move_position((TaxiPoint){0,10,0},(TaxiPoint){0,1000,0},0,query,&f,&r)==0);
    assert(r.position.y==10 && r.queries==4); /* Y contributes to subdivisions only. */
    f.kind=1;f.height=654;
    assert(taxi_move_position((TaxiPoint){0,0,0},(TaxiPoint){100,0,0},0,query,&f,&r)==0);
    assert(!r.collided && r.position.x==100);
    f.height=655;
    assert(taxi_move_position((TaxiPoint){0,0,0},(TaxiPoint){100,0,0},0,query,&f,&r)==0);
    assert(r.collided && r.position.x==0 && r.normal.x==-256);
    f.kind=2;
    assert(taxi_move_position((TaxiPoint){-100,10,50},(TaxiPoint){200,0,0},0,query,&f,&r)==0);
    assert(r.collided && r.position.x==-64 && r.position.y==10 && r.position.z==50);
    assert(r.normal.x==-256 && r.normal.z==0);
    static TaxiTrig trig;trig.cosine[0]=256;
    TaxiVehicleBody body={.position={-100,10,50},.impact={.speed_544=45520}};
    TaxiImpactEffects effects;
    assert(taxi_move_vehicle(&body,(TaxiPoint){200,0,0},0,0,&trig,query,&f,&r,&effects)==0);
    assert(body.position.x==-64 && body.state_e84==4096);
    assert(body.impact.speed_544==34140 && body.impact.state_74c==-524288);
    assert(effects.projected_x==8192 && effects.effect_slot==1);
    f.kind=0;f.height=INT_MIN;
    assert(taxi_move_position((TaxiPoint){0,10,0},(TaxiPoint){600,0,0},1,query,&f,&r)==0);
    assert(f.last.y==10);
    assert(taxi_move_position((TaxiPoint){0,10,0},(TaxiPoint){600,0,0},0,query,&f,&r)==0);
    assert(f.last.y==INT_MIN);
    f.kind=-1;
    assert(taxi_move_position((TaxiPoint){0},(TaxiPoint){0},0,query,&f,&r)==-1);
    puts("PASS: native position movement, step rounding, wall pushout and height threshold.");
}
