#include "menu_footer_layout.h"
int taxi_menu_footer_layout(uint16_t height,uint32_t callback,int confirm,int back,
    uint16_t text_width,uint16_t icon_width,uint16_t back_width,TaxiMenuFooterLayout *out){
    if(!out)return -1;
    TaxiMenuFooterLayout result={.middle_count=(callback==0x080172e1||callback==0x08019b7d)?4:5,
        .strip_x=callback==0x080172e1?-28:0,.strip_y=(int32_t)height-16};
    int32_t x=4;
    if(confirm){
        TaxiMenuFooterPrompt *p=result.prompt+result.prompts++;
        *p=(TaxiMenuFooterPrompt){4,x,(int32_t)height-11,x+icon_width,(int32_t)height-12};
        x+=icon_width+text_width;
    }
    if(back){
        TaxiMenuFooterPrompt *p=result.prompt+result.prompts++;
        *p=(TaxiMenuFooterPrompt){5,x,(int32_t)height-11,x+back_width,(int32_t)height-12};
    }
    *out=result;return 0;
}
