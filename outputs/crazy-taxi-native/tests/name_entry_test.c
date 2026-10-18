#include "../src/name_entry.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
int main(int argc,char **argv) {
    assert(argc==3);TaxiNameLayout layout;assert(!taxi_name_layout_load(&layout,argv[2]));
    assert(layout.slots[0][0]==88&&layout.slots[1][0]==120&&layout.slots[2][0]==153&&layout.slots[2][1]==104);uint8_t chars[43];assert(!taxi_name_characters_load(chars,argv[1]));
    assert(chars[0]=='A'&&chars[25]=='Z'&&chars[42]=='^');
    TaxiNameRingGlyph glyph;
    assert(!taxi_name_ring_glyph(&layout,0,0,8,160,&glyph));
    assert(glyph.character==0&&glyph.visible&&glyph.affine&&glyph.x==120&&glyph.y==144&&glyph.scale==512);
    assert(!taxi_name_ring_glyph(&layout,0,0,7,160,&glyph));
    assert(glyph.character==42&&glyph.visible&&!glyph.affine&&glyph.x==80&&glyph.y==136&&glyph.scale==256);
    assert(!taxi_name_ring_glyph(&layout,42,0,9,160,&glyph));
    assert(glyph.character==0&&glyph.x==144&&glyph.scale==256);
    assert(!taxi_name_ring_glyph(&layout,0,16,7,160,&glyph));
    assert(glyph.x==104&&glyph.affine&&glyph.scale==384);
    assert(!taxi_name_ring_glyph(&layout,0,-16,9,160,&glyph));
    assert(glyph.x==136&&glyph.affine&&glyph.scale==384);
    assert(!taxi_name_ring_glyph(&layout,0,0,0,160,&glyph));
    assert(glyph.x==-32&&glyph.visible&&!glyph.affine&&glyph.character==35);
    assert(!taxi_name_ring_glyph(&layout,0,20,6,160,&glyph));
    assert(!glyph.visible); /* Source only affine-draws ring slots7..9. */
    assert(taxi_name_ring_glyph(&layout,43,0,8,160,&glyph)==-1);
    for(unsigned i=0;i<256;i++)assert(taxi_name_entry_eligible(0,(uint8_t)i)==(i>=1&&i<=10));
    assert(!taxi_name_entry_eligible(32,1));
    TaxiNameEntry s;taxi_name_entry_begin(&s,&layout,160);
    assert(!taxi_name_entry_step(&s,chars,1,0,0,0,1));assert(!s.move);
    assert(taxi_name_entry_step(&s,chars,0,1,0,0,0)==TAXI_NAME_MOVE);
    for(int i=0;i<8;i++)taxi_name_entry_step(&s,chars,0,0,0,0,0);
    assert(s.selection==0&&s.angle==16);
    taxi_name_entry_step(&s,chars,0,0,0,0,0);assert(s.selection==42&&!s.move);
    assert(taxi_name_entry_step(&s,chars,0,0,1,0,0)==TAXI_NAME_FADE_OUT);
    assert(!taxi_name_entry_step(&s,chars,1,0,0,0,0));
    taxi_name_entry_begin(&s,&layout,160);
    for(int letter=0;letter<3;letter++) {
        taxi_name_entry_step(&s,chars,0,0,1,0,0);
        assert(s.flight_x==120*256&&s.flight_y==144*256&&s.velocity_y==-640);
        for(int i=0;i<15;i++)assert(!taxi_name_entry_step(&s,chars,0,0,0,0,0));
        assert(taxi_name_entry_step(&s,chars,0,0,0,0,0)==TAXI_NAME_ACCEPT);
    }
    assert(s.flight_x==153*256&&s.flight_y==104*256);
    assert(s.length==3&&s.name[0]=='A'&&s.name[2]=='A'&&s.automatic==1);
    for(int i=0;s.automatic&&i<500;i++)taxi_name_entry_step(&s,chars,0,0,0,0,0);
    assert(s.selection==42&&!s.automatic);
    assert(taxi_name_entry_step(&s,chars,0,0,0,1,0)==TAXI_NAME_DELETE&&s.length==2&&s.name[2]==0);
    /* Source25b3c..25b66 separately handles length3 + selectedEND.
       All42 accepted symbols must auto-scroll toward42, wait for a new
       confirm request, then prioritize END over simultaneous deletion. */
    for(unsigned selected=0;selected<42;selected++) {
        taxi_name_entry_begin(&s,&layout,160);s.selection=(uint8_t)selected;
        for(unsigned letter=0;letter<3;letter++) {
            taxi_name_entry_step(&s,chars,0,0,1,0,0);
            for(unsigned tick=0;tick<16;tick++)taxi_name_entry_step(&s,chars,0,0,0,0,0);
        }
        assert(s.length==3&&s.automatic==(selected>21?-1:1));
        unsigned calls=0;
        while(s.automatic&&calls<430) {
            assert(!taxi_name_entry_step(&s,chars,1,1,1,1,0));calls++;
        }
        assert(calls==10*(selected>21?42-selected:selected+1));
        assert(s.selection==42&&!s.move&&!s.angle&&!s.exiting);
        assert(!taxi_name_entry_step(&s,chars,0,0,0,0,0));
        assert(s.length==3&&s.name[2]==chars[selected]);
        assert(taxi_name_entry_step(&s,chars,0,0,1,1,0)==TAXI_NAME_FADE_OUT);
        assert(s.exiting&&s.length==3&&s.name[2]==chars[selected]);
        assert(!taxi_name_entry_step(&s,chars,0,0,1,1,0));
    }
    puts("Original name entry: top10/mode eligibility, fade gate, nine-call selection wrap, sixteen-call letter acceptance, automatic END and deletion passed; UI/save pending.");
}
