#include "menu_input.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
int main(void){
    for(unsigned physical=0;physical<1024;physical++)
        for(unsigned recorded=0;recorded<1024;recorded++)
            for(unsigned gate=0;gate<2;gate++)
                for(unsigned mode=0;mode<2;mode++){
                    unsigned expected=0;
                    for(unsigned bit=0;bit<10;bit++){
                        unsigned held=mode&&bit!=0&&bit!=3?(recorded>>bit)&1:gate?0:(physical>>bit)&1;
                        expected|=held<<bit;
                    }
                    assert(taxi_menu_input_source_mask(physical,gate,mode,recorded)==expected);
                }
    TaxiMenuInput bank={0};
    taxi_menu_input_step(&bank,taxi_menu_input_source_mask(9,0,1,64),1);
    assert(bank.pressed_mask==73&&bank.held[0]&&bank.held[3]&&bank.held[6]);
    taxi_menu_input_step(&bank,taxi_menu_input_source_mask(9,1,1,128),1);
    assert(bank.pressed_mask==128&&!bank.held[0]&&!bank.held[3]&&!bank.held[6]&&bank.held[7]);
    puts("PASS original physical gate/recorded override: all4194304 masks and fresh/release bank transitions.");
}
