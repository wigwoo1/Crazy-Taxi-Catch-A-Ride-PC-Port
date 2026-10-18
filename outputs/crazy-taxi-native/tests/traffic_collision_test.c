#include "traffic_collision.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
    for(int sign=-1;sign<=1;sign+=2)for(int axis=0;axis<2;axis++){
        TaxiTrafficActor a={0};a.flags=7;a.x=axis?0:sign*100;a.z=axis?sign*100:0;
        a.impulse_x=9;a.impulse_z=11;
        assert(taxi_traffic_collision(&a,0,0,0,0,0,1)==0);
        assert(a.x==(axis?0:sign*768)&&a.z==(axis?sign*768:0));
        assert(a.impulse_x==9+(axis?0:sign*4267)&&a.impulse_z==11+(axis?sign*4267:0));
        assert(a.flags==6&&a.collision_timer==120);
    }
    TaxiTrafficActor a={0};a.x=100;
    assert(taxi_traffic_collision(&a,0,0,1000000,0,0,1)==0&&a.x==1152);
    memset(&a,0,sizeof a);TaxiTrafficActor saved=a;
    assert(taxi_traffic_collision(&a,0,0,0,0,0,1)==-1&&memcmp(&a,&saved,sizeof a)==0);
    for(int magnitude=0;magnitude<=2000;magnitude++)for(int steps=0;steps<=8;steps++){
        memset(&a,0,sizeof a);a.impulse_x=magnitude;a.impulse_z=-magnitude;
        int expected=magnitude;for(int i=0;i<steps;i++)expected=expected*243/256;
        if(expected<=16)expected=0;
        int32_t dx,dz;
        assert(taxi_traffic_impulse_displacement(&a,steps,3,&dx,&dz)==0);
        assert(a.impulse_x==expected&&a.impulse_z==-expected);
        assert(dx==expected*3/60&&dz==-(expected*3/60));
    }
    TaxiAiPathPoint points[2]={{0,0,0,0},{100,100,0,0}};
    TaxiAiPathAsset road={points,2};memset(&a,0,sizeof a);
    a.offset_x=17;a.offset_z=-16;a.flags=6;
    int32_t dx,dz;
    assert(taxi_traffic_rejoin_position(&a,&road,1,&dx,&dz)==1);
    assert(a.x==18&&a.z==-16&&a.offset_x==1&&a.offset_z==0&&dx==-17&&dz==16&&a.flags==6);
    assert(taxi_traffic_rejoin_position(&a,&road,1,&dx,&dz)==0);
    assert(a.x==3&&a.z==0&&a.offset_x==0&&a.offset_z==0&&a.flags==7);
    puts("Original traffic radial collision axis/cap/state fixtures PASS");return 0;
}
