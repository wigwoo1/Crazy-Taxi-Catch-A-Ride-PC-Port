#include "../src/ground.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
static void put16(uint8_t *p,uint16_t n) { p[0]=(uint8_t)n;p[1]=(uint8_t)(n>>8); }
static void put32(uint8_t *p,int32_t n) {
    for(int i=0;i<4;i++) p[i]=(uint8_t)((uint32_t)n>>(i*8));
}
static void vtx(uint8_t *p,int32_t x,int32_t y,int32_t z) { put32(p,x);put32(p+4,y);put32(p+8,z); }
int main(void) {
    uint8_t v[8*16]={0},f[2*24]={0},refs[4]={0,0,1,0};
    vtx(v,0,0,0);vtx(v+16,0,0,100);vtx(v+32,100,0,100);vtx(v+48,100,0,0);
    for(int j=0;j<4;j++) { memcpy(v+64+j*16,v+j*16,16);put32(v+64+j*16+4,100); }
    for(int j=0;j<2;j++) {
        uint8_t *p=f+j*24;p[0]=(uint8_t)(7+j);p[3]=p[6]=128;
        for(int k=0;k<4;k++) put16(p+8+k*2,(uint16_t)(j*4+k));
    }
    TaxiGroundHit hit;
    uint8_t materials[9*28]={0};TaxiObstacleHit obstacle;
    assert(taxi_obstacle_candidates(v,8,f,2,materials,9,refs,2,(TaxiPoint){25,20,75},&obstacle)==0);
    materials[7*28+20]=1;
    assert(taxi_obstacle_candidates(v,8,f,2,materials,9,refs,2,(TaxiPoint){25,20,75},&obstacle)==2);
    assert(obstacle.polygon==0 && obstacle.height==INT_MAX && obstacle.edge_count==4);
    assert(taxi_obstacle_candidates(v,8,f,2,materials,9,refs,2,(TaxiPoint){75,20,25},&obstacle)==2);
    assert(taxi_obstacle_candidates(v,8,f,2,materials,7,refs,2,(TaxiPoint){25,20,75},&obstacle)==-1);
    materials[7*28+20]=0;f[3]=f[6]=64;
    assert(taxi_obstacle_candidates(v,8,f,2,materials,9,refs,2,(TaxiPoint){25,20,75},&obstacle)==1);
    assert(obstacle.height==0 && obstacle.polygon==65535);
    f[3]=f[6]=128;
    assert(taxi_ground_candidates(v,8,f,2,refs,2,(TaxiPoint){75,20,25},&hit)==1);
    assert(hit.height==0 && hit.polygon==0 && hit.material==7 && hit.normal.y==-4096);
    assert(hit.contact_valid && hit.contact_value==0);
    assert(taxi_ground_candidates(v,8,f,2,refs,2,(TaxiPoint){25,80,75},&hit)==1);
    assert(hit.height==100 && hit.polygon==1); /* Other quad half and upper deck. */
    assert(taxi_ground_candidates(v,8,f,2,refs,2,(TaxiPoint){50,50,50},&hit)==1);
    assert(hit.polygon==0); /* Ties retain first cell reference. */
    int32_t height=123;
    assert(taxi_ground_height_candidates(v,8,f,2,refs,2,(TaxiPoint){25,80,75},&height)==1&&height==100);
    assert(taxi_ground_height_candidates(v,8,f,2,refs,2,(TaxiPoint){50,50,50},&height)==1&&height==0);
    assert(taxi_ground_height_candidates(v,8,f,2,refs,2,(TaxiPoint){25,INT_MIN,75},&height)==1&&height==0);
    assert(taxi_ground_height_candidates(v,8,f,2,refs,2,(TaxiPoint){150,0,50},&height)==0&&height==INT_MIN);
    uint8_t invalid_refs[2]={2,0};height=123;
    assert(taxi_ground_height_candidates(v,8,f,2,invalid_refs,1,(TaxiPoint){0,0,0},&height)==-1&&height==123);
    assert(taxi_ground_candidates(v,8,f,2,refs,2,(TaxiPoint){25,INT_MIN,75},&hit)==1);
    assert(hit.height==0 && hit.material==65535);
    assert(!hit.contact_valid);
    assert(taxi_ground_candidates(v,8,f,2,refs,2,(TaxiPoint){150,0,50},&hit)==0);
    assert(hit.height==INT_MIN);
    f[3]=0;f[2]=128;f[6]=0;f[5]=128;
    assert(taxi_ground_candidates(v,8,f,2,refs,1,(TaxiPoint){75,0,25},&hit)==0); /* Wall. */
    f[2]=64;f[3]=128;f[5]=0;f[6]=128;
    assert(taxi_ground_candidates(v,8,f,2,refs,1,(TaxiPoint){25,0,75},&hit)==1);
    assert(hit.height==12); /* Signed division truncates 12.5 toward zero. */
    put16(f+14,65535);
    assert(taxi_ground_candidates(v,8,f,2,refs,1,(TaxiPoint){25,0,75},&hit)==1);
    assert(taxi_ground_candidates(v,8,f,2,refs,1,(TaxiPoint){75,0,25},&hit)==0);
    refs[1]=128;
    assert(taxi_ground_candidates(v,8,f,2,refs,1,(TaxiPoint){75,0,25},&hit)==0);
    refs[0]=2;refs[1]=0;
    assert(taxi_ground_candidates(v,8,f,2,refs,1,(TaxiPoint){0,0,0},&hit)==-1);
    uint32_t value=123;
    put32(v+12,0);put32(v+16+12,0);put32(v+32+12,10000);
    assert(taxi_ground_contact(25,50,v,v+16,v+32,&value)==1);
    assert(value==2500u*256);
    put32(v+12,65500);put32(v+16+12,65500);put32(v+32+12,100);
    assert(taxi_ground_contact(25,50,v,v+16,v+32,&value)==1);
    assert(value==65534u*256); /* Short path across wrap, not through 32768. */
    assert(taxi_ground_contact(0,0,v,v+16,v+32,&value)==1);
    assert(value==65500u*256); /* Collapsed span at a vertex. */
    put32(v+12,0);put32(v+16+12,0);put32(v+32+12,32768);
    assert(taxi_ground_contact(25,50,v,v+16,v+32,&value)==1);
    assert(value==8192u*256); /* Exactly half a cycle does not unwrap. */
    value=123;
    assert(taxi_ground_contact(0,200,v,v+16,v+32,&value)==0 && value==123);
    assert(taxi_ground_contact(25,50,v,v+16,v+32,NULL)==0);
    put32(v+16+8,0);put32(v+32+8,0);
    assert(taxi_ground_contact(25,0,v,v+16,v+32,&value)==0 && value==123);
    refs[0]=0;put16(f+8,8);
    assert(taxi_ground_candidates(v,8,f,2,refs,1,(TaxiPoint){0,0,0},&hit)==-1);
    puts("PASS: native ground height, deck selection, normals, quads, triangles and invalid references.");
}
