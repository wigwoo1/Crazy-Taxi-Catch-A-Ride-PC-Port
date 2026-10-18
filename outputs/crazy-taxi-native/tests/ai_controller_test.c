#include "ai_controller.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(int argc,char **argv){
    assert(argc==3);TaxiAiPathAsset path={0};assert(!taxi_ai_path_load(&path,argv[1]));
    FILE *reference=fopen(argv[2],"rb");assert(reference);
    for(unsigned phase=0;phase<65536;phase+=17)for(unsigned parity=0;parity<2;parity++){
        TaxiAiVehicle v={.position={3,777,4},.speed_544=100000,.phase_378=phase<<8,.command_4=255,.command_c=99};
        uint32_t counter=parity;int32_t expected[3]={0,777,0};
        int result=taxi_ai_controller_step(&v,0,&counter,&path);
        int32_t expected_fields[7];assert(fread(expected_fields,4,7,reference)==7);
        assert(result==expected_fields[0]&&v.position[0]==expected_fields[1]&&v.position[2]==expected_fields[2]);
        assert(v.heading_1c==expected_fields[3]&&v.heading_20==expected_fields[4]&&v.command_4==expected_fields[5]&&counter==(uint32_t)expected_fields[6]);
        assert(result>=0&&v.position[1]==777&&v.speed_544==100000&&v.phase_378==phase<<8);
        assert(!v.command_c&&(v.command_4&240)==240&&!(v.command_4&12));
        if(result){assert(counter==parity+1);assert(!taxi_ai_path_target(path.points,path.count,parity?4096:61440,expected));assert(v.position[0]==expected[0]&&v.position[2]==expected[2]&&v.heading_1c==v.heading_20);}
        else assert(counter==parity&&v.position[0]==3&&v.position[2]==4);
    }
    TaxiAiVehicle v={0},before=v;uint32_t counter=11;
    TaxiDriveWorld world={0},player={0};TaxiCommand cmd={0};
    world.world.contact_378=2048u<<12;world.world.height.position.y=777;world.world.speed_544=100000;
    world.driver.power.boost_558=909;cmd.bytes[4]=255;cmd.bytes[12]=cmd.bytes[13]=255;counter=0;
    assert(taxi_ai_world_prepare(&world,&player,&cmd,&counter,&path)==1&&counter==1);
    assert(world.world.height.position.y==777&&world.driver.power.boost_558==909&&world.world.contact_378==(2048u<<12));
    assert(!cmd.bytes[12]&&!cmd.bytes[13]&&(cmd.bytes[4]&240)==240);
    counter=11;
    assert(taxi_ai_controller_step(&v,0,&counter,NULL)==-1&&!memcmp(&v,&before,sizeof v)&&counter==11);
    assert(fgetc(reference)==EOF);fclose(reference);
    taxi_ai_path_free(&path);puts("PASS source AI controller: independent whole-controller reference, original path targets, relocation parity, retained Y/speed/phase and invalid-path rollback.");
}
