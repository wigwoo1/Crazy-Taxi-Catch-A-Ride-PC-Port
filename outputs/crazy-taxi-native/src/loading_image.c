#include "loading_image.h"
#include "palette_fade.h"
#include <string.h>
int taxi_loading_image_placement(uint8_t output[38400],const uint8_t image[4096],unsigned y0){
 if(!output||!image||(y0!=0&&y0!=60&&y0!=120))return -1;
 unsigned rows=160-y0;if(rows>64)rows=64;
 for(unsigned y=0;y<rows;y++)for(unsigned x=0;x<64;x++){
  uint8_t index=image[y*64+x];if(index)output[(y0+y)*240+x]=index;
 }
 return 0;
}
void taxi_loading_image(uint8_t output[38400],const uint8_t image[4096]){
 for(unsigned placement=0;placement<3;placement++)(void)taxi_loading_image_placement(output,image,placement*60);
}
void taxi_loading_background(uint8_t output[153600], const uint8_t image[4096],
    const uint16_t palette[256], uint8_t clear_index, int16_t progress) {
    uint8_t indices[38400];
    memset(indices, clear_index, sizeof indices);
    taxi_loading_image(indices, image);
    for (unsigned i = 0; i < 38400; ++i) {
        uint16_t color = taxi_palette_fade_cap_rgb555(palette[indices[i]], progress);
        for (unsigned channel = 0; channel < 3; ++channel)
            output[i * 4 + channel] = (uint8_t)(((color >> (channel * 5)) & 31) * 255 / 31);
        output[i * 4 + 3] = 255;
    }
}
