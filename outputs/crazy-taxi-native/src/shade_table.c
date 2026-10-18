#include "shade_table.h"
int taxi_palette_extrema(const uint16_t palette[256],uint8_t *darkest,uint8_t *brightest) {
    if(!palette || !darkest || !brightest)return 0;
    unsigned minimum=94,maximum=0,min_index=1,max_index=1;
    for(unsigned i=1;i<256;++i) {
        unsigned value=palette[i];
        unsigned sum=(value&31)+((value>>5)&31)+((value>>10)&31);
        if(sum<minimum) {minimum=sum;min_index=i;}
        if(i==1 || sum>maximum) {maximum=sum;max_index=i;}
    }
    *darkest=(uint8_t)min_index;*brightest=(uint8_t)max_index;return 1;
}
#include <string.h>
int taxi_shade_table_material(uint8_t lookup[2560], const uint8_t *materials,
    size_t count, uint32_t index) {
    unsigned low = index & 255;
    if (!materials || low >= 128 || low >= count) return 0;
    memset(lookup, materials[low * 20 + 15], 2560);
    return 1;
}
static int absolute(int v) { return v < 0 ? -v : v; }
static int floor_eighth(int v) { return v >= 0 ? v / 8 : -((-v + 7) / 8); }
static uint8_t nearest(const uint16_t *palette, const int target[3]) {
    int best = 0x7fffffff;
    uint8_t index = 0;
    for (int i = 0; i < 256; ++i) {
        int distance = 0;
        for (int c = 0; c < 3; ++c)
            distance += absolute(((palette[i] >> (c * 5)) & 31) - target[c]);
        if (distance < best) { best = distance; index = (uint8_t)i; }
    }
    return index;
}
int taxi_shade_table_build(uint8_t lookup[2560], const uint16_t palette[256],
    uint32_t depth, uint32_t packed_color) {
    if (!depth || !palette) return 0;
    int target[3] = {(int)((packed_color >> 19) & 31),
        (int)((packed_color >> 11) & 31), (int)((packed_color >> 3) & 31)};
    for (int i = 0; i < 256; ++i) {
        for (int step = 2; step <= 8; ++step) {
            int color[3];
            for (int c = 0; c < 3; ++c) {
                int source = (palette[i] >> (c * 5)) & 31;
                color[c] = source + floor_eighth((target[c] - source) * step);
            }
            lookup[step * 256 + i] = nearest(palette, color);
        }
        lookup[i] = lookup[256 + i] = (uint8_t)i;
    }
    memset(lookup + 2304, nearest(palette, target), 256);
    return 1;
}

int taxi_palette_nearest_pair(const uint16_t palette[256],unsigned red,unsigned green,unsigned blue,uint32_t *packed){
 if(!palette||!packed||red>31||green>31||blue>31)return 0;
 unsigned selected[2]={0,0};
 for(unsigned pass=0;pass<2;pass++){
  unsigned best=1024;
  for(unsigned index=1;index<256;index++){
   if(pass&&index==selected[0])continue;
   uint16_t color=palette[index];
   unsigned distance=(unsigned)absolute((int)red-(color&31))+
    (unsigned)absolute((int)green-((color>>5)&31))+
    (unsigned)absolute((int)blue-((color>>10)&31));
   if(distance<best){best=distance;selected[pass]=index;}
  }
 }
 *packed=(selected[0]<<24)|(selected[1]<<16)|(selected[0]<<8)|selected[1];return 1;
}

int taxi_palette_nearest_index(const uint16_t palette[256],unsigned red,
    unsigned green,unsigned blue,uint8_t *index){
    if(!palette||!index||red>31||green>31||blue>31)return 0;
    unsigned best=1024,selected=0;
    for(unsigned i=0;i<256;i++){
        unsigned p=palette[i],r=p&31,g=(p>>5)&31,b=(p>>10)&31;
        unsigned distance=(red>r?red-r:r-red)+(green>g?green-g:g-green)+(blue>b?blue-b:b-blue);
        if(distance<best){best=distance;selected=i;}
    }
    *index=(uint8_t)selected;return 1;
}
