#include "attract_graphics.h"
int taxi_attract_graphics_prefix(TaxiAttractGraphicsCall call,void *context){
 if(!call)return -1;
 const uint32_t assets[6]={0x0805a338,1,0,0,0,0};
 const uint32_t first[6]={0,32,0,0x084097b8,1,0};
 const uint32_t second[6]={1,32,1,0,0xffffffff,0};
 if(call(context,0x08027994,assets)||call(context,0x0800fec8,first)||call(context,0x0800fec8,second))return -1;
 return 0;
}
