#include "../src/destination_guidance.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void put(uint8_t *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(i*8));}
int main(int argc,char **argv){
    assert(argc==4);TaxiGuidanceRoute loaded;
    assert(!taxi_guidance_route_load(&loaded,argv[1])&&loaded.count==66);
    TaxiGuidanceRoute saved=loaded;
    assert(taxi_guidance_route_load(&loaded,"missing-guidance-test.ctg")==-1&&!memcmp(&saved,&loaded,sizeof(loaded)));
    assert(!taxi_guidance_route_load(&loaded,argv[2])&&loaded.count==0);
    assert(!taxi_guidance_route_load(&loaded,argv[3])&&loaded.count==0);
    assert(taxi_guidance_frame_step(0,1,1)==0); /* Original loses upwardfraction. */
    assert(taxi_guidance_frame_step(1,0,1)==0);
    assert(taxi_guidance_frame_step(0,1,4)==1&&taxi_guidance_frame_step(31,0,4)==0);
    assert(taxi_guidance_frame_step(0,31,1)==31);
    assert(taxi_guidance_frame_step(0,16,4)==31&&taxi_guidance_frame_step(16,0,4)==17);
    TaxiPoint points[3]={{0,0,0},{768,0,1024},{0,0,1024}};
    TaxiGuidanceSegment segments[3];assert(!taxi_guidance_segments(segments,points,3));
    assert(segments[0].length==1280&&segments[0].unit_x==153&&segments[0].unit_z==204);
    assert(segments[1].length==768&&segments[1].unit_x==-256&&segments[1].unit_z==0);
    assert(segments[2].x1==0&&segments[2].z1==0&&segments[2].unit_z==-256);
    points[1]=points[0];assert(!taxi_guidance_segments(segments,points,3));
    assert(segments[0].length==0&&segments[0].unit_x==0&&segments[0].unit_z==0);
    assert(taxi_guidance_segments(segments,points,81)==-1);
    assert(taxi_destination_palette(501)==0&&taxi_destination_palette(500)==1);
    assert(taxi_destination_palette(251)==1&&taxi_destination_palette(250)==2);
    assert(taxi_destination_palette(151)==2&&taxi_destination_palette(150)==3);
    assert(taxi_destination_palette(101)==3&&taxi_destination_palette(100)==4);
    assert(taxi_destination_vertical_scale(101)==256&&taxi_destination_vertical_scale(100)==256);
    assert(taxi_destination_vertical_scale(50)==320&&taxi_destination_vertical_scale(0)==384);
    uint8_t d[48]={0};put(d+24,3*256);put(d+32,4*256);put(d+36,2*256);
    assert(taxi_destination_distance(0,d,0,0)==3);
    assert(taxi_destination_distance(0,d,3*256,4*256)==0);
    assert(taxi_destination_distance(-1,d,0,0)==-1);
    /* Shift before squaring, including negative-floor behavior. */
    put(d+24,255);put(d+32,0);put(d+36,0);
    assert(taxi_destination_distance(0,d,0,0)==0);
    assert(taxi_destination_distance(0,d,256,0)==1);
    TaxiTrig trig={0};trig.cosine[0]=256;trig.sine[3072]=-256;
    TaxiAngleTable table={0};table.values[256]=512;int32_t angle=99;
    put(d+24,256);assert(!taxi_destination_angle(0,d,(TaxiPoint){0},0,&trig,&table,&angle));assert(angle==1024);
    assert(!taxi_destination_angle(0,d,(TaxiPoint){0},1024,&trig,&table,&angle));assert(angle==2048);
    assert(!taxi_destination_angle(-1,d,(TaxiPoint){0},0,0,0,&angle)&&angle==-1);
    angle=99;assert(taxi_destination_angle(0,d,(TaxiPoint){0},0,0,&table,&angle)==-1&&angle==99);
    TaxiPoint loop[4]={{0,0,0},{1024,0,0},{1024,0,1024},{0,0,1024}};
    TaxiGuidanceSegment road[4];assert(!taxi_guidance_segments(road,loop,4));
    put(d+24,512);put(d+32,1024);table.values[128]=302;
    TaxiPoint position={512,0,0};
    assert(!taxi_guidance_route_angle(road,4,0,d,position,position,0,&trig,&table,&angle));
    assert(angle==3794); /* Equal circuit lengths choose backward, source branch. */
    road[1].length-=256;
    assert(!taxi_guidance_route_angle(road,4,0,d,position,position,0,&trig,&table,&angle));
    assert(angle==302); /* Shorter forward arc points to next segment end. */
    assert(!taxi_guidance_route_angle(NULL,0,-1,NULL,position,position,0,&trig,&table,&angle));
    assert(angle==4095); /* Caller masks original -1 absent target. */
    assert(taxi_guidance_scale_step(256,384,1)==257);
    assert(taxi_guidance_scale_step(383,384,4)==384);
    assert(taxi_guidance_scale_step(384,256,1)==383);
    assert(taxi_guidance_scale_step(258,256,4)==256);
    assert(taxi_guidance_scale_step(300,300,10)==300);
    assert(taxi_guidance_scale_step(300,384,0)==300);
    assert(taxi_guidance_scale_step(65535,1,1)==0); /* Signed -1 to0. */
    puts("Native guidance: distance, route angle, palettes, scale follow/overshoot and sentinel passed");return 0;
}
