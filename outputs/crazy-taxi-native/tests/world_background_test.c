#include "../src/world_background.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void) {
    uint8_t source[38400],actual[38400],expected[38400];
    for(unsigned i=0;i<38400;++i)source[i]=(uint8_t)(i/240+i%240);
    for(unsigned angle=0;angle<4096;++angle) {
        int signed_pitch=angle>=2048?(int)angle-4096:(int)angle;
        int half=signed_pitch/2;
        assert(taxi_world_background_pitch((int32_t)angle)==
            (half>142?142:half< -142?-142:half));
        memset(actual,99,sizeof actual);memset(expected,99,sizeof expected);
        int p=angle>2048?(int)angle-4096:(int)angle;
        int q=0;while(q*4>p)--q;while((q+1)*4<=p)++q;
        int start=64-q;if(start<0)start=0;if(start>160)start=160;
        unsigned yaw=angle*7,rotate=((4096-((yaw*4)%4096))%4096)*60/4096*4;
        for(unsigned y=12;y<148;++y)for(unsigned x=8;x<232;++x) {
            unsigned sy=(unsigned)start+y-12;
            expected[y*240+x]=sy<160?source[sy*240+(rotate+x-8)%240]:
                (uint8_t)(0x44332211u>>(8*((x-8)%4)));
        }
        assert(!taxi_world_background(actual,source,sizeof source,160,angle,yaw,8,12,232,148,0x44332211));
        assert(!memcmp(actual,expected,sizeof actual));
    }
    assert(taxi_world_background(actual,source,1,160,0,0,0,0,240,160,0)==-1);
    uint8_t indices[256],layers[256],before[256];
    for(unsigned i=0;i<256;i++)indices[i]=(uint8_t)i;
    assert(!taxi_world_bitmap_layers(layers,indices,256,4,1));
    for(unsigned i=0;i<256;i++)assert(layers[i]==(i?2:5));
    assert(!taxi_world_bitmap_layers(layers,NULL,256,3,1));
    for(unsigned i=0;i<256;i++)assert(layers[i]==2);
    assert(!taxi_world_bitmap_layers(layers,indices,256,4,0));
    for(unsigned i=0;i<256;i++)assert(layers[i]==5);
    memcpy(before,layers,sizeof before);
    assert(taxi_world_bitmap_layers(layers,indices,256,5,1)==-1);
    assert(taxi_world_bitmap_layers(layers,NULL,256,4,1)==-1);
    assert(taxi_world_bitmap_layers(layers,indices,256,4,2)==-1);
    assert(!memcmp(before,layers,sizeof layers));
    assert(!taxi_world_bitmap_layers(indices,indices,256,4,1));
    for(unsigned i=0;i<256;i++)assert(indices[i]==(i?2:5));
    assert(!taxi_world_bitmap_layers(NULL,NULL,0,4,1));
    /* Actual recovered background copy retains0 independently of fill. */
    memset(source,0,sizeof source);
    assert(!taxi_world_background(actual,source,sizeof source,160,0,0,0,0,240,160,0x01010101));
    assert(!taxi_world_bitmap_layers(expected,actual,38400,4,1));
    for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++)
        assert(expected[y*240+x]==(y<96?5:2));
    puts("PASS bitmap coverage: all256 indices, direct-color mode3, disabled BG2, original copied index0/fill1, aliasing and invalid rollback.");
    puts("PASS original background pitch, yaw wrap, viewport and word fill across4096 angles.");
}
