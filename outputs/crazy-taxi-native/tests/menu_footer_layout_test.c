#ifdef NDEBUG
#undef NDEBUG
#endif
#include "menu_footer_layout.h"
#include <assert.h>
#include <stdio.h>
int main(void){
    TaxiMenuFooterLayout l;
    for(unsigned mask=0;mask<4;mask++){
        assert(!taxi_menu_footer_layout(160,0x08017a25,mask&1,mask&2,38,15,15,&l));
        assert(l.middle_count==5&&l.strip_x==0&&l.strip_y==144);
        assert(l.prompts==((mask&1)!=0)+((mask&2)!=0));
        for(unsigned i=0;i<l.prompts;i++){
            assert(l.prompt[i].icon_y==149&&l.prompt[i].text_y==148);
            assert(l.prompt[i].text_x==l.prompt[i].icon_x+15);
        }
        if(mask==3)assert(l.prompt[1].icon_x==57);
        if(mask==2)assert(l.prompt[0].icon_x==4&&l.prompt[0].descriptor==5);
    }
    assert(!taxi_menu_footer_layout(160,0x080172e1,1,1,0,0,0,&l)&&l.middle_count==4&&l.strip_x==-28);
    assert(!taxi_menu_footer_layout(160,0x08019b7d,1,1,0,0,0,&l)&&l.middle_count==4&&!l.strip_x);
    puts("PASS original callback-owned footer strip count and independent prompt layout.");return 0;
}
