#undef NDEBUG
#include "../src/challenge_completion.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
static TaxiNormalAngles angles;
int main(void){
    for(unsigned profile=0;profile<2;profile++){
        uint8_t settings[512]={0};TaxiHeightGlobals globals={.takeoff_1544={0,100,0}};
        TaxiHeightState state={.position={40000,100,0},.scale_e84=4096,.flags_28=24,
            .body={.denominator_754=57344,.denominator_758=28672}};
        TaxiWorldContact contact={.height=100,.normal={0,4096,0}};
        TaxiHeightInput input={.settings=128,.delta=1};TaxiHeightEffects effects;
        assert(!taxi_height_update(&state,&globals,&contact,&input,&angles,&effects));
        assert(effects.landed&&effects.completion_call&&globals.flag_1697);
        assert(taxi_challenge_distance_complete(settings,(uint8_t)profile,3,&globals,&effects)==1);
        assert(globals.flag_153d==1);
        unsigned offset=208*profile+108;
        uint32_t word=(uint32_t)settings[offset]|(uint32_t)settings[offset+1]<<8|
            (uint32_t)settings[offset+2]<<16|(uint32_t)settings[offset+3]<<24;
        assert((word>>15)==156&&((word>>8)&127)==25&&((word>>5)&3)==3&&((word>>7)&1)==profile);
        for(unsigned i=0;i<512;i++)if(i<offset||i>=offset+4)assert(!settings[i]);
        uint8_t retained[512];memcpy(retained,settings,512);
        assert(!taxi_height_update(&state,&globals,&contact,&input,&angles,&effects));
        assert(!effects.completion_call);
        assert(taxi_challenge_distance_complete(settings,(uint8_t)profile,3,&globals,&effects)==0);
        assert(!memcmp(retained,settings,512)&&globals.flag_153d==1);
        assert(taxi_challenge_distance_complete(settings,2,3,&globals,&effects)==-1);
        assert(!memcmp(retained,settings,512));
    }
    TaxiChallengeFailure failure={.ended1697=1};uint8_t buttons[8]={0};
    buttons[0]=1;
    assert(!taxi_challenge_failure_step(&failure,1,0,0,1,1,buttons)&&!failure.phase16bc);
    assert(!taxi_challenge_failure_step(&failure,0,1,0,1,1,buttons)&&!failure.phase16bc);
    assert(!taxi_challenge_failure_step(&failure,0,0,0,1,1,buttons)&&failure.hold16be==120);
    for(unsigned i=0;i<119;i++)assert(!taxi_challenge_failure_step(&failure,0,0,0,1,1,buttons));
    assert(taxi_challenge_failure_step(&failure,0,0,1,1,1,buttons)==TAXI_CHALLENGE_FAILURE_VOICE);
    assert(!failure.outer1ea8&&failure.ended1697&&!failure.hold16be);
    assert(taxi_challenge_failure_step(&failure,0,0,0,1,1,buttons)==TAXI_CHALLENGE_RETRY_VOICE);
    assert(failure.outer1ea8==1&&!failure.ended1697&&failure.fade_flags==1);
    failure=(TaxiChallengeFailure){.phase16bc=1,.ended1697=1};buttons[7]=1;
    assert(!taxi_challenge_failure_step(&failure,0,0,0,1,1,buttons)&&failure.selection153b==1&&!failure.outer1ea8);
    buttons[7]=0;
    assert(!taxi_challenge_failure_step(&failure,0,0,0,0,1,buttons));
    assert(failure.outer1ea8==2&&!failure.ended1697&&failure.fade_flags==1);
    for(unsigned rank=0;rank<4;rank++){
     TaxiChallengeSuccess success={.ended1697=1};
     assert(!taxi_challenge_success_step(&success,(uint8_t)rank,1,0,1,1)&&!success.phase16bc);
     assert(taxi_challenge_success_step(&success,(uint8_t)rank,0,0,1,1)==TAXI_CHALLENGE_SUCCESS_VOICE);
     assert(success.hold16be==300);
     for(unsigned tick=1;tick<=300;tick++){
      unsigned event=taxi_challenge_success_step(&success,(uint8_t)rank,0,0,1,1);
      unsigned expected=tick==121&&rank?(rank==1?TAXI_CHALLENGE_BEST_VOICE:TAXI_CHALLENGE_PLACED_VOICE):0;
      assert(event==expected);
      assert(success.outer1ea8==(tick==300?2:0));
     }
     assert(!success.ended1697&&success.fade_flags==1);
    }
    puts("PASS source success flow:300-tick hold,rank0..3 voice threshold180->179,automatic outer2 return andfade1.");
    puts("PASS source failure flow:120-tick hold,audio gate,selection priority,Retry/Return outer states andfade1; render/audio dispatch pending.");
    puts("PASS recovered challenge landing -> distance record -> returned rank for both settings groups; no repeated completion.");
    return 0;
}
