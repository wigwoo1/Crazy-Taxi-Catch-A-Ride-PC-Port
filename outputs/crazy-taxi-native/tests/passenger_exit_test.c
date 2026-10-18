#include "../src/passenger_exit.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void put(uint8_t *p,int32_t v) {for(unsigned i=0;i<4;i++) p[i]=(uint8_t)((uint32_t)v>>(8*i));}
int main(void) {
    uint8_t vertices[128]={0},polygons[48]={0},cell[32]={0};const uint8_t *cells[]={cell};
    for(unsigned layer=0;layer<2;layer++) {
        const int coords[4][2]={{0,0},{0,2000},{2000,2000},{2000,0}};
        for(unsigned i=0;i<4;i++) {uint8_t *p=vertices+(layer*4+i)*16;put(p,coords[i][0]);put(p+4,layer*100);put(p+8,coords[i][1]);polygons[layer*24+8+i*2]=(uint8_t)(layer*4+i);}
        polygons[layer*24+3]=polygons[layer*24+6]=128;
    }
    put(cell,2);cell[30]=1;
    TaxiWorldAsset world={.storage=cell,.vertices=vertices,.polygons=polygons,.cells=cells,
        .nv=8,.np=2,.nx=1,.nz=1,.sx=2000,.sz=2000};
    TaxiTrig trig={0};trig.cosine[0]=256;trig.cosine[2048]=-256;
    trig.sine[1024]=256;trig.sine[3072]=-256;TaxiPoint exit={0};
    assert(taxi_passenger_exit_position(&world,&trig,(TaxiPoint){800,900,800},0,80,&exit)==1);
    assert(exit.x==1184&&exit.z==800&&exit.y==100); /* TaxiY is not height query seed. */
    assert(taxi_passenger_exit_position(&world,&trig,(TaxiPoint){800,900,800},1024,20,&exit)==1);
    assert(exit.x==800&&exit.z==1184&&exit.y==0);
    assert(taxi_passenger_exit_position(&world,&trig,(TaxiPoint){800,900,800},2048,INT32_MIN,&exit)==1);
    assert(exit.x==416&&exit.z==800&&exit.y==0);
    assert(taxi_passenger_exit_position(&world,&trig,(TaxiPoint){1900,0,800},0,0,&exit)==0&&exit.y==INT32_MIN);
    TaxiPoint before=exit;world.storage=0;
    assert(taxi_passenger_exit_position(&world,&trig,(TaxiPoint){0},0,0,&exit)==-1&&!memcmp(&exit,&before,sizeof(exit)));
    puts("Native passenger exit: original camera field, side-offset signs, retained-height deck selection and no-hit passed");return 0;
}
