#include "distance_bitmap.h"
#include "indexed_preview_scale.h"
#include "destination_preview_border.h"
/* Fixed original31860 dimensions; validated owners are part of this API. */
void taxi_distance_bitmap_border(uint8_t output[4096],uint8_t color){
 (void)taxi_destination_preview_border(output,4096,64,64,color);
}
void taxi_distance_bitmap_sample(uint8_t output[4096],const uint8_t source[38400]){
 (void)taxi_indexed_preview_scale(source,38400,240,160,output,4096,64,64);
}
