#include "world_background.h"
#include <string.h>
int32_t taxi_world_background_pitch(int32_t camera_pitch) {
    unsigned wrapped=(uint32_t)camera_pitch&4095;
    int32_t signed_pitch=wrapped>=2048?(int32_t)wrapped-4096:(int32_t)wrapped;
    int32_t half=signed_pitch/2;
    if(half>142)half=142;
    if(half< -142)half=-142;
    return half;
}
int taxi_world_background_pattern(const TaxiIndexedAsset *asset,int32_t descriptor,uint32_t *pattern){
    TaxiIndexedTexture texture;
    if(!pattern||descriptor<0||taxi_indexed_asset_texture(asset,(unsigned)descriptor,&texture)||
       !texture.width||!texture.height||texture.width>255||texture.height>255||
       texture.size<(size_t)texture.width*texture.height)return -1;
    *pattern=(uint32_t)texture.pixels[texture.width*(texture.height-1)]*0x01010101u;return 0;
}
int taxi_world_background_scene(uint8_t frame[38400],const TaxiIndexedAsset *asset,
    int32_t descriptor,int32_t pitch,int32_t yaw,unsigned left,unsigned top,
    unsigned right,unsigned bottom) {
    TaxiIndexedTexture texture;
    if(descriptor<0 || taxi_indexed_asset_texture(asset,(unsigned)descriptor,&texture)
        || texture.width!=240 || !texture.height || texture.height>160)return -1;
    uint32_t fill;if(taxi_world_background_pattern(asset,descriptor,&fill))return -1;
    return taxi_world_background(frame,texture.pixels,texture.size,texture.height,
        taxi_world_background_pitch(pitch),yaw,left,top,right,bottom,fill);
}
int taxi_world_background(uint8_t frame[38400],const uint8_t *texture,
    size_t size,unsigned height,int32_t pitch,int32_t yaw,
    unsigned left,unsigned top,unsigned right,unsigned bottom,uint32_t fill) {
    if(!frame||!texture||height>160||size<(size_t)height*240||
       left>right||right>240||top>bottom||bottom>160||(left&3)||(right&3))return -1;
    unsigned angle=(uint32_t)pitch&4095;
    int signed_angle=angle>2048?(int)angle-4096:(int)angle;
    /* ARM ASR rounds negative pitch down, independent of host signed shift. */
    int quarter=signed_angle>=0?signed_angle/4:-((-signed_angle+3)/4);
    int first=64-quarter;
    if(first<0)first=0;
    if(first>160)first=160;
    unsigned scroll=((((0u-(uint32_t)yaw)<<2)&4095)*60>>12)*4;
    unsigned split=240-scroll;
    if(split>right-left)split=right-left;
    unsigned copied=first<(int)height?height-(unsigned)first:0;
    if(copied>bottom-top)copied=bottom-top;
    for(unsigned row=0;row<bottom-top;++row) {
        uint8_t *dest=frame+(top+row)*240+left;
        if(row<copied) {
            const uint8_t *src=texture+((unsigned)first+row)*240;
            memcpy(dest,src+scroll,split);
            memcpy(dest+split,src,right-left-split);
        }else {
            for(unsigned x=0;x<right-left;++x)dest[x]=(uint8_t)(fill>>(8*(x&3)));
        }
    }
    return 0;
}
int taxi_world_bitmap_layers(uint8_t *layers,const uint8_t *indices,size_t count,
    unsigned mode,int enabled){
    if((mode!=3&&mode!=4)||(enabled!=0&&enabled!=1)||
       (!layers&&count)||(mode==4&&!indices&&count))return -1;
    for(size_t i=0;i<count;i++)layers[i]=(uint8_t)(!enabled||
        (mode==4&&!indices[i])?5:2);
    return 0;
}

int taxi_world_clear_rect(uint8_t frame[38400],int32_t x,int32_t y,
    int32_t width,int32_t height,uint32_t pattern){
    if(!frame||width<0||width>240||height<0||height>160)return -1;
    /* Widened arithmetic is exact within the supported original viewport;
       it also avoids undefined host overflow for far offscreen positions. */
    int64_t right=(int64_t)x+width,bottom=(int64_t)y+height;
    if(x>239||y>159||right<=0||bottom<=0)return 0;
    int32_t left=x<0?0:x,top=y<0?0:y;
    int32_t end_x=right>240?240:(int32_t)right;
    int32_t end_y=bottom>160?160:(int32_t)bottom;
    for(int32_t row=top;row<end_y;row++)
        for(int32_t col=left;col<end_x;col++)
            frame[row*240+col]=(uint8_t)pattern;
    return 0;
}
