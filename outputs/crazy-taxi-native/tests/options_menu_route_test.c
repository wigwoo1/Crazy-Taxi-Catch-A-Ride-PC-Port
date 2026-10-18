#ifdef NDEBUG
#undef NDEBUG
#endif
#include "options_menu_route.h"
#include <assert.h>
#include <stdio.h>
int main(void){
    for(unsigned row=0;row<256;row++){
        assert(taxi_options_menu_confirm_prompt((uint8_t)row)==(row>=3&&row<=4));
        for(unsigned reason=0;reason<256;reason++){
            TaxiOptionsMenuRoute r=taxi_options_menu_route((uint8_t)row,(uint8_t)reason,0x08017919);
            assert(r.restore_palette_mask==(reason==2?4096:0));
            if(reason==2)assert(r.callback==0x080174dd);
            else if(row==3)assert(r.callback==0x08017a25);
            else if(row==4)assert(r.callback==0x08017bb9);
            else if(row==5)assert(r.callback==0x08018cdd);
            else assert(r.callback==0x08017919);
        }
    }
    /* The existing callback is caller-owned, including unknown values. */
    assert(taxi_options_menu_route(0,1,0x12345678).callback==0x12345678);
    puts("PASS source Options callback routing: all byte selections/exits, cancellation palette restore and confirm prompts.");
    return 0;
}
