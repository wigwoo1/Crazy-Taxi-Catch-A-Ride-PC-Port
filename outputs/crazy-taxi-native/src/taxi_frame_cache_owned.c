#include "taxi_frame_cache_owned.h"
int taxi_frame_cache_owned(TaxiSpriteReplacementOwner *o,uint32_t *previous,uint32_t selected,uint8_t driver,uint8_t immediate){
 if(!previous)return -1;
 if(*previous==selected)return 0;
 if(selected>63||taxi_frame_copy_owned(o,driver,(uint16_t)selected,immediate))return -1;
 *previous=selected;return 1;
}
