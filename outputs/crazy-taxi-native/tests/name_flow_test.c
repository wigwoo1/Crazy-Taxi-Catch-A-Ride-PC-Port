#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/name_flow.h"
#include "../src/menu_input.h"
int main(int argc,char **argv) {
    assert(argc==3);TaxiNameLayout layout;uint8_t chars[43];TaxiNameFlow s;
    assert(!taxi_name_layout_load(&layout,argv[1]));
    assert(!taxi_name_characters_load(chars,argv[2]));
    assert(!taxi_name_flow_begin(&s,&layout,160,0,0));
    for(int i=0;i<33;i++) {
        assert(!taxi_name_flow_step(&s,chars,1,1,1,1,1));
        assert(!s.entry.move&&!s.entry.flying&&!s.entry.length);
        assert(s.palette_cap==(i<32?i:31));
    }
    assert(!s.fade.active_177b&&!s.complete);
    /* Holding a logical request here is an explicit fixture, not an input
       implementation. Source flight still takes16 calls even with delta60. */
    assert(!taxi_name_flow_step(&s,chars,60,0,0,1,0));
    for(int i=0;i<15;i++)assert(!taxi_name_flow_step(&s,chars,60,0,0,0,0));
    assert(s.entry.flying&&!s.entry.length);
    assert(taxi_name_flow_step(&s,chars,60,0,0,0,0)==TAXI_NAME_ACCEPT);
    assert(s.entry.length==1&&s.entry.name[0]=='A');
    s.entry.selection=42;
    assert(taxi_name_flow_step(&s,chars,1,0,0,1,0)==TAXI_NAME_FADE_OUT);
    assert(s.entry.exiting&&s.palette_cap==31&&!s.complete);
    for(int i=0;i<32;i++)assert(!taxi_name_flow_step(&s,chars,1,1,1,1,1));
    assert(s.complete&&s.palette_cap==0&&!s.fade.active_177b);
    TaxiNameFlow saved=s;assert(!taxi_name_flow_step(&s,chars,1,1,1,1,1));
    assert(!memcmp(&s,&saved,sizeof s));
    assert(!taxi_name_flow_begin(&s,&layout,160,1,2));
    for(int i=0;i<64;i++)taxi_name_flow_step(&s,chars,1,0,0,1,0);
    assert(s.fade.active_177b&&!s.entry.flying);
    taxi_name_flow_step(&s,chars,1,0,0,1,0);
    assert(!s.fade.active_177b&&!s.entry.flying);
    taxi_name_flow_step(&s,chars,1,0,0,1,0);assert(s.entry.flying);
    /* Source's separate length3 END branch retains the entered initials
       through the exit fade and never auto-confirms upon reaching END. */
    assert(!taxi_name_flow_begin(&s,&layout,160,0,0));
    for(unsigned i=0;i<33;i++)taxi_name_flow_step(&s,chars,1,0,0,0,0);
    s.entry.selection=41;
    for(unsigned letter=0;letter<3;letter++) {
        taxi_name_flow_step(&s,chars,1,0,0,1,0);
        for(unsigned tick=0;tick<16;tick++)taxi_name_flow_step(&s,chars,1,0,0,0,0);
    }
    for(unsigned i=0;i<10;i++)taxi_name_flow_step(&s,chars,1,0,0,0,0);
    assert(s.entry.selection==42&&!s.entry.exiting&&s.entry.length==3);
    assert(taxi_name_flow_step(&s,chars,1,0,0,1,1)==TAXI_NAME_FADE_OUT);
    for(unsigned i=0;i<32;i++)taxi_name_flow_step(&s,chars,1,0,0,0,0);
    assert(s.complete&&s.entry.length==3&&s.entry.name[0]==' '&&s.entry.name[2]==' ');
    /* UI consumes the preceding frame's requests;26318 then samples keys.
       Holding A through both repeat periods must accept only one letter. */
    assert(!taxi_name_flow_begin(&s,&layout,160,0,0));
    for(unsigned i=0;i<33;i++)taxi_name_flow_step(&s,chars,1,0,0,0,0);
    TaxiMenuInput input={0};
    for(unsigned i=0;i<150;i++) {
        taxi_name_flow_step(&s,chars,1,input.held[4],input.held[5],input.pressed[0]||input.pressed[3],input.pressed[1]);
        taxi_menu_input_step(&input,1,1);
    }
    assert(s.entry.length==1&&!s.entry.flying&&!s.entry.exiting);
    taxi_menu_input_step(&input,0,1);taxi_menu_input_step(&input,1,1);
    taxi_name_flow_step(&s,chars,1,0,0,input.pressed[0],0);assert(s.entry.flying);
    for(unsigned i=0;i<16;i++)taxi_name_flow_step(&s,chars,1,0,0,0,0);
    assert(s.entry.length==2);
    assert(taxi_name_flow_begin(NULL,&layout,160,0,0)==-1);
    assert(taxi_name_flow_begin(&s,NULL,160,0,0)==-1);
    puts("Native name flow: source fade gating/order, call-based flight, retained intervals, single END exit and reset passed; device input/persistence pending.");
    return 0;
}
