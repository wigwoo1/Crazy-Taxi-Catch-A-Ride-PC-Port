#ifdef NDEBUG
#undef NDEBUG
#endif
#include "options_menu_control.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
    TaxiGameSettings settings={{0,32,64,0x44,0,50,0,0}};
    TaxiOptionsMenuControl state={0};TaxiMenuInput input={0};TaxiOptionsMenuControlEffect e;
    /* Same frame moves onto Time Diff, decreases only low nibble, cancels. */
    input.repeat[7]=input.repeat[5]=input.repeat[1]=1;
    assert(!taxi_options_menu_control(&state,&settings,&input,0,0,1,&e));
    assert(state.row==1&&state.exiting==2&&settings.bytes[3]==0x43);
    assert(e.count==3&&e.sounds[0]==3&&e.sounds[1]==7&&e.sounds[2]==6&&e.fade_flags==4096);
    /* Navigation precedes confirm: move Traffic->Sound, A wins over B. */
    state=(TaxiOptionsMenuControl){2,0};input=(TaxiMenuInput){0};
    input.repeat[7]=input.repeat[0]=input.repeat[1]=1;
    assert(!taxi_options_menu_control(&state,&settings,&input,0,0,1,&e));
    assert(state.row==3&&state.exiting==1&&e.count==2&&e.sounds[0]==3&&e.sounds[1]==4);
    /* Recorded gate still permits selected-row adjustments. */
    state=(TaxiOptionsMenuControl){0,0};input.repeat[4]=1;
    assert(!taxi_options_menu_control(&state,&settings,&input,0,1,1,&e));
    assert(state.row==0&&!state.exiting&&settings.bytes[5]==55&&e.count==1&&e.sounds[0]==7);
    TaxiGameSettings before=settings;
    assert(!taxi_options_menu_control(&state,&settings,&input,1,0,1,&e));
    assert(!e.count&&!memcmp(&settings,&before,sizeof settings));
    for(unsigned row=0;row<256;row++)for(unsigned keys=0;keys<4;keys++){
        state=(TaxiOptionsMenuControl){(uint8_t)row,0};input=(TaxiMenuInput){0};
        input.repeat[0]=keys&1;input.repeat[1]=keys&2;
        assert(!taxi_options_menu_control(&state,&settings,&input,0,0,0,&e));
        int confirm=(keys&1)&&row>2&&row<128&&row!=5;
        assert(state.exiting==(confirm?1:(keys&2)?2:0));
        assert(e.fade_flags==(state.exiting?4096:0)&&!e.count);
    }
    state=(TaxiOptionsMenuControl){4,0};input=(TaxiMenuInput){0};input.repeat[7]=1;
    assert(!taxi_options_menu_control(&state,&settings,&input,0,0,0,&e));
    assert(state.row==5&&!state.exiting);
    input=(TaxiMenuInput){0};input.repeat[0]=1;
    assert(!taxi_options_menu_control(&state,&settings,&input,0,0,0,&e));
    assert(state.row==5&&!state.exiting&&!e.fade_flags);
    puts("PASS Options controller: original rows plus native PC texture row, original route ordering and cancel behavior.");
    return 0;
}
