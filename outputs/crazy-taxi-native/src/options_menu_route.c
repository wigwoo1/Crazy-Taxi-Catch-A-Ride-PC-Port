#include "options_menu_route.h"
TaxiOptionsMenuRoute taxi_options_menu_route(uint8_t row,uint8_t reason,uint32_t callback){
    TaxiOptionsMenuRoute result={callback,0};
    if(reason==2){result.callback=0x080174dd;result.restore_palette_mask=4096;}
    else if(row==3)result.callback=0x08017a25;
    else if(row==4)result.callback=0x08017bb9;
    else if(row==5)result.callback=0x08018cdd;
    return result;
}
int taxi_options_menu_confirm_prompt(uint8_t row){return row==3||row==4;}
