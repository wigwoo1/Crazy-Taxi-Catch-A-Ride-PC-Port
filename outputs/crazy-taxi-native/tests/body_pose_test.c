#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/body_pose.h"
static TaxiNormalAngles angles;
int main(int argc,char **argv) {
    angles.angle[0]=-1024;angles.angle[8192]=1024;
    if(argc==2) {
        FILE *f=fopen(argv[1],"rb");assert(f);uint8_t b[8193*2];
        assert(!fseek(f,0x7ec9ec,SEEK_SET)&&fread(b,1,sizeof b,f)==sizeof b);fclose(f);
        assert(!taxi_normal_angles_decode(&angles,b,sizeof b));
        assert(angles.angle[0]==-1024&&angles.angle[4096]==0&&angles.angle[8192]==1024);
        for(unsigned i=1;i<8193;i++) assert(angles.angle[i]>=angles.angle[i-1]);
    }
    TaxiBodyPose v={.position_734=140,.position_740=140,.denominator_754=57344,.denominator_758=28672};
    assert(!taxi_body_springs(&v));assert(v.position_734==110&&v.velocity_73c==-10&&v.response_704==110);
    assert(v.position_740==110&&v.velocity_748==-10&&v.response_70c==110);
    v.position_734=v.position_740=1;v.velocity_73c=v.velocity_748=-10;
    assert(!taxi_body_springs(&v));assert(v.position_734==0&&v.velocity_73c==0&&v.position_740==-9&&v.velocity_748==-10);
    v.denominator_758=0;TaxiBodyPose before=v;
    assert(taxi_body_springs(&v)==-1&&!memcmp(&before,&v,sizeof v));
    v=(TaxiBodyPose){0};TaxiPoint n={-4096,0,4096};
    assert(!taxi_body_align(&v,n,1,&angles));
    assert(v.velocity_730==255&&v.velocity_732==255&&v.orientation_6f4.x==511&&v.orientation_6f4.z==511);
    assert(!taxi_body_align(&v,n,1,&angles));assert(v.velocity_730==382&&v.orientation_6f4.x==1021);
    before=v;assert(taxi_body_align(&v,(TaxiPoint){4097,0,0},1,&angles)==-1&&!memcmp(&v,&before,sizeof v));
    v=(TaxiBodyPose){.velocity_730=32767};
    assert(!taxi_body_align(&v,n,1,&angles));assert(v.velocity_730==-32512&&v.orientation_6f4.x==-32256);
    TaxiWorldContact contact={.flags=0x80,.red=1,.green=2,.blue=3,.contact_value=123};
    TaxiGroundHit hit={.height=INT32_MIN};TaxiWorldContact old=contact;
    assert(!taxi_world_contact(&contact,&hit)&&!memcmp(&contact,&old,sizeof contact));
    hit=(TaxiGroundHit){.height=50,.normal={0,4096,0},.polygon=17,.material=123};
    assert(taxi_world_contact(&contact,&hit));
    assert(contact.height==50&&contact.material==123&&contact.flags==0x81&&contact.contact_value==123);
    assert(!contact.red&&!contact.green&&!contact.blue);
    hit.normal=(TaxiPoint){0};hit.contact_valid=1;hit.contact_value=456;
    assert(!taxi_world_contact(&contact,&hit)&&contact.contact_value==456);
    puts("PASS: native body springs and normal alignment, sign crossings, halfword wrap and lookup bounds.");return 0;
}

