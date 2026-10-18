#include "world_polygon_stamp.h"
int taxi_world_polygon_stamp(uint32_t *cache,size_t count,uint32_t generation,unsigned reference){
 if(!cache||reference>=count||reference>=0x8000)return -1;
 if(cache[reference]==generation)return 0;
 cache[reference]=generation;return 1;
}
/* Original38A8 retains TST flags across non-flag-setting index masking. */
int taxi_world_special_stamp(uint32_t *cache,size_t count,uint32_t generation,unsigned reference){
 if(reference>0xffffu||!(reference&0x8000u))return -1;
 if(reference&0x4000u)return 0;
 return taxi_world_polygon_stamp(cache,count,generation,reference&0x3fffu);
}
